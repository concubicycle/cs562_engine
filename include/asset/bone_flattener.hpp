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
                std::function<void(const aiNode*, T*, T*, bone_flattener<T>&)> mapper) 
                : _storage(storage)
                , _storage_size(storage_size)
                , _mapper(mapper)
        {
            const auto* root = scene->mRootNode;

            auto base_transform = root->mTransformation;


            Eigen::Matrix4f eigenmat = 
                Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>(&base_transform.a1);
            _base_inverse = eigenmat.transpose().inverse();

            load_nodes_recurse(root);


            for (size_t mesh_i = 0; mesh_i < scene->mNumMeshes; ++mesh_i)
            {
                auto& mesh = scene->mMeshes[mesh_i];
                for (size_t bone_i = 0; bone_i < mesh->mNumBones; ++bone_i)
                {
                    auto* bone = mesh->mBones[bone_i];
                    _name_to_bone[bone->mName.data] = bone;
                }
            }

            map_recurse(root);
        }

        static void count_nodes(const aiNode* node, size_t& accumulator)
        {
            accumulator++;
            for (size_t i = 0; i < node->mNumChildren; ++i)
                count_nodes(node->mChildren[i], accumulator);
        }


        const std::unordered_map<std::string, T*>& name_to_node() const { return _name_to_node; }
        const std::unordered_map<std::string, size_t>& name_to_index() const { return _name_to_index; };
        T* find_node(const std::string& name) { return _name_to_node.find(name)->second; }
        size_t find_node_index(const std::string& name) { return _name_to_index.find(name)->second; }
        T* root() { return _root; }
        size_t bone_count() const { return _bone_count; }
        Eigen::Matrix4f base_inverse() { return _base_inverse; }

        Eigen::Matrix4f find_offset_for_bone(std::string name) 
        {
            if (_name_to_bone.find(name) == _name_to_bone.end())
                return Eigen::Matrix4f(1.f);

            auto& source = _name_to_bone.find(name)->second->mOffsetMatrix;
            return glm::transpose(glm::make_mat4(&source.a1)); 
        }


    private:
        size_t _bone_count{0};

        T* _storage;
        T* _root;
        size_t _storage_size;
        std::function<void(const aiNode*, T*, T*, bone_flattener<T>&)> _mapper;

        Eigen::Matrix4f _base_inverse;

        std::unordered_map<std::string, T*> _name_to_node;
        std::unordered_map<std::string, const aiNode*> _name_to_ai_node;      
        std::unordered_map<std::string, aiBone*> _name_to_bone;
        std::unordered_map<std::string, size_t> _name_to_index;
        std::unordered_map<std::string, T*> _name_to_parent;

        void load_nodes_recurse(const aiNode* ai_node, T* parent = nullptr)
        {            
            T* user_node = allocate_and_map_to(ai_node, parent);

            if (parent == nullptr)
                _root = user_node;

            for (size_t i = 0; i < ai_node->mNumChildren; ++i)
            {
                auto* ai_child = ai_node->mChildren[i];
                load_nodes_recurse(ai_child, user_node);
            }
        }

        void map_recurse(const aiNode* ai_node)
        {
            T* user_node = find_node(ai_node->mName.data);
            T* parent = _name_to_parent[ai_node->mName.data];
            _mapper(ai_node, user_node, parent, *this);

            for (size_t i = 0; i < ai_node->mNumChildren; ++i)
            {
                auto* ai_child = ai_node->mChildren[i];
                map_recurse(ai_child);
            }
        }

        T* allocate_and_map_to(const aiNode* ai_node, T* parent)
        {
            if (_bone_count == _storage_size)
            {
                throw new std::runtime_error("Could not allocate bone - out of space.");                
            }
            
            auto* user_node = _storage + _bone_count;
            _name_to_node.insert(std::make_pair(ai_node->mName.data, user_node));
            _name_to_ai_node.insert(std::make_pair(ai_node->mName.data, ai_node));
            _name_to_parent.insert(std::make_pair(ai_node->mName.data, parent));     //[ai_node->mName.data] = parent;
            _name_to_index.insert(std::make_pair(ai_node->mName.data, _bone_count++));
            return user_node;
        }
    };
}

#endif //WIZARDENGINE_BONE_FLATTENER_HPP
