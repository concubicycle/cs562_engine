//
// Created by sava on 2020-02-19.
//

#ifndef WIZARDENGINE_BONE_FLATTENER_HPP
#define WIZARDENGINE_BONE_FLATTENER_HPP

#include <functional>
#include <vector>
#include <cstdint>
#include <assimp/scene.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <spdlog/spdlog.h>

namespace asset
{
  /**
   * This class takes an assimp bone node graph and re-builds the graph in an array.
   * The indices of this array can then reliably be used as bone ids in various parts
   * of the model loading code.
   *
   * The ultimate goal is to have the bone's index in an array be the bone id.
   * @tparam T bone type to map to
   */
  template<typename T>
  class bone_flattener // _\m/
  {
  public:
    /**
     * Constructor
     * @param scene assimp scene, containing the animation info
     * @param storage an array of whatever type the client uses to represent bone
     * @param storage_size size of storage array
     * @param mapper function with header:
     *  `void mapper(aiNode* assimp_node, T* client_node, T* parent_of_client_node)`
     *  parent_of_client_node is nullptr for root.
     */
    bone_flattener(
      const aiScene* scene,
      T* storage,
      size_t storage_size,
      std::function<void(aiNode*, T*, T*, bone_flattener<T>&)> mapper)
      : _scene(scene)
      , _storage(storage)
      , _storage_size(storage_size)
      , _mapper(mapper)
    {
      auto* root = scene->mRootNode;

      auto base_transform = root->mTransformation;
      float* fptr = (float*)&(root->mTransformation.a1);
      Eigen::Matrix4f eigenmat = Eigen::Map<Eigen::Matrix4f>(fptr).matrix();
      _base_inverse = eigenmat.transpose().inverse();

      load_nodes_recurse(root);
      build_node_to_mesh_bone();
      map_recurse(root);
    }

    static void count_nodes(const aiNode* node, size_t& accumulator)
    {
      accumulator++;
      for (size_t i = 0; i < node->mNumChildren; ++i)
        count_nodes(node->mChildren[i], accumulator);
    }

    void build_node_to_mesh_bone()
    {
      for (size_t mesh_i = 0; mesh_i < _scene->mNumMeshes; ++mesh_i)
      {
        auto* mesh = _scene->mMeshes[mesh_i];
        

        for (size_t bone_i = 0; bone_i < mesh->mNumBones; ++bone_i)
        {
          auto* bone = mesh->mBones[bone_i];
          auto& nodes = _nodes_by_name.at(bone->mName.C_Str());

          if (nodes.size() > 1)
            spdlog::warn("More than one node with same name in mesh.");

          auto* node = nodes.at(0);
          _node_to_mesh_bone[node] = bone;
        }
      }
    }
        
    size_t find_node_index(aiNode* node) { return _node_to_index.at(node); }
    const std::vector<aiNode*> find_nodes(std::string name)
    { 
      return _nodes_by_name.at(name);
    }
    T* find_user_node(aiNode* node) { return _ai_to_user_node.at(node); }

    T* root() { return _root; }
    size_t bone_count() const { return _bone_count; }
    Eigen::Matrix4f base_inverse() { return _base_inverse; }

    Eigen::Matrix4f find_offset_for_node(aiNode* node)
    {
      if (_node_to_mesh_bone.find(node) == _node_to_mesh_bone.end())
        return Eigen::Matrix4f::Identity();

      auto& source = _node_to_mesh_bone.at(node)->mOffsetMatrix;
      float* fptr = (float*)(&source.a1);
      Eigen::Map<Eigen::Matrix4f> mat(fptr);      
      return mat.eval().transpose();
    }


  private:
    size_t _bone_count{ 0 };

    const aiScene* _scene;
    T* _storage;
    T* _root;
    size_t _storage_size;

    /**
    Callback Params:
      Assimp Node : aiNode*,
      User Type Node : T*,
      Parent User Type Node : T*,
      this flattener : bone_flattener<T>
    */
    std::function<void(aiNode*, T*, T*, bone_flattener<T>&)> _mapper;

    Eigen::Matrix4f _base_inverse;

    std::unordered_map<aiNode*, T*> _ai_to_user_node;
    std::unordered_map<aiNode*, aiBone*> _node_to_mesh_bone;
    std::unordered_map<aiNode*, size_t> _node_to_index;
    std::unordered_map<aiNode*, T*> _node_to_parent;

    // nodes can actually have duplicate names, so we need to track/warn
    std::unordered_map<std::string, std::vector<aiNode*>> _nodes_by_name;

    void load_nodes_recurse(aiNode* ai_node, T* parent = nullptr)
    {
      T* user_node = allocate_and_map_to(ai_node, parent);

      if (parent == nullptr)
        _root = user_node;

      for (size_t i = 0; i < ai_node->mNumChildren; ++i)
      {
        auto* ai_child = ai_node->mChildren[i];
        load_nodes_recurse(ai_child, user_node);
      }

      std::string name(ai_node->mName.C_Str());
      _nodes_by_name[name].push_back(ai_node);
    }

    void map_recurse(aiNode* ai_node)
    {
      T* user_node = _ai_to_user_node.at(ai_node);
      T* parent = _node_to_parent.at(ai_node);
      _mapper(ai_node, user_node, parent, *this);

      for (size_t i = 0; i < ai_node->mNumChildren; ++i)
      {
        auto* ai_child = ai_node->mChildren[i];
        map_recurse(ai_child);
      }
    }

    T* allocate_and_map_to(aiNode* ai_node, T* parent)
    {
      if (_bone_count == _storage_size)
        throw new std::runtime_error("Could not allocate bone - out of space.");

      T* user_node = _storage + _bone_count;

      _ai_to_user_node.insert(std::make_pair(ai_node, user_node));
      _node_to_parent.insert(std::make_pair(ai_node, parent));
      _node_to_index.insert(std::make_pair(ai_node, _bone_count));

      _bone_count++;
      return user_node;
    }
  };
}

#endif //WIZARDENGINE_BONE_FLATTENER_HPP
