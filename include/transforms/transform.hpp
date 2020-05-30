#ifndef __TRANSFORM_HPP_
#define __TRANSFORM_HPP_

#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
#include <ecs/entity.hpp>

#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Geometry> 
#include <optional>

namespace transforms
{
	typedef Eigen::Transform<float, 3, Eigen::Affine> affine_transform;	

	/**
		Functions return references, allowing caller to modify data. 
	**/
	struct transform : ecs::component<transform>
	{
		void calculate_local()
		{			
			_rotation = Eigen::AngleAxis(_rotation_euler[2], Eigen::Vector3f::UnitZ())
				* Eigen::AngleAxis(_rotation_euler[1], Eigen::Vector3f::UnitY())
				*Eigen::AngleAxis(_rotation_euler[0], Eigen::Vector3f::UnitX());

			_local_to_parent = _translation * _rotation * Eigen::Scaling(_scale);  
		}

		void calculate_local_to_world(const affine_transform& parent)
		{
			_local_to_world = parent * _local_to_parent;
			_adjoint_transpose = _local_to_world.matrix().adjoint().transpose();
			_local_to_world_matrix = _local_to_world.matrix();
		}

		void calculate_local_to_world()
		{
			_local_to_world = _local_to_parent;
			_adjoint_transpose = _local_to_world.matrix().adjoint().transpose();
			_local_to_world_matrix = _local_to_world.matrix();
		}

		affine_transform& local_to_world()
		{
			_is_dirty = true;
			return _local_to_world;
		}

		affine_transform& local_to_parent()
		{
			_is_dirty = true;
			return _local_to_parent;
		}

		Eigen::Translation3f& position()
		{
			_is_dirty = true;
			return _translation;
		}
		
		Eigen::Vector3f& rotation_euler()
		{
			_is_dirty = true;
			return _rotation_euler;
		}

		Eigen::Vector3f& scale()
		{
			_is_dirty = true;
			return _scale;
		}

		std::optional<ecs::entity*>& parent() 
		{ 
			_is_dirty = true;
			return _parent; 
		}

		// hierarchy
		const std::optional<ecs::entity*>& parent() const { return _parent; }

		// matrices
		const affine_transform& local_to_world() const { return _local_to_world; }
		const affine_transform& local_to_parent() const { return _local_to_parent; }
		const Eigen::Matrix4f& adjoint_transpose() const { return _adjoint_transpose; }
		const Eigen::Matrix4f& model_matrix() const { return _local_to_world_matrix; }

		// trs
		const Eigen::Translation3f& position() const { return _translation; }
		const Eigen::Quaternionf& rotation() const { return _rotation; }
		const Eigen::Vector3f& rotation_euler() const { return _rotation_euler; }
		const Eigen::Vector3f& scale() const { return _scale; }

		Eigen::Vector3f world_position() const 
		{ 
			return _local_to_world.translation();
		}

		bool is_dirty() const { return _is_dirty; }
		void set_dirty() { _is_dirty = true; }
		void set_clean() { _is_dirty = false; }
		
		void add_child(ecs::entity* child)
		{
			_children.emplace_back(child->id(), child);
		}

		void remove_child(entity_id id)
		{
			_children.erase(
				std::remove_if(
					_children.begin(), 
					_children.end(), 
					[id](const std::pair<entity_id, ecs::entity*>& pair) {
						return pair.first == id;
					}),
				_children.end());
		}

		const std::vector<std::pair<entity_id, ecs::entity*>>& children() const
		{
			return _children;
		}

	private:		
		affine_transform _local_to_world{ affine_transform::Identity() };
		affine_transform _local_to_parent{ affine_transform::Identity() };
		Eigen::Matrix4f _adjoint_transpose;
		Eigen::Matrix4f _local_to_world_matrix;
		Eigen::Translation3f _translation{ Eigen::Translation3f::Identity() };
		Eigen::Quaternionf _rotation { Eigen::Quaternionf::Identity() };
		Eigen::Vector3f _scale{ 1.f, 1.f, 1.f };		
		Eigen::Vector3f _rotation_euler{ 0.f, 0.f, 0.f };
		std::optional<ecs::entity*> _parent;

		// it may become unsafe to access a dangling ecs::entity*,
		// but systems can check the id first
		std::vector<std::pair<entity_id, ecs::entity*>> _children;


		bool _is_dirty;
	};
}

#endif