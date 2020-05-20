#ifndef __TRANSFORM_HPP_
#define __TRANSFORM_HPP_

#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
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
			_local_to_parent = _translation * _rotation * Eigen::Scaling(_scale);
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

		Eigen::Quaternionf& rotation()
		{
			_is_dirty = true;
			return _rotation;
		}

		Eigen::Vector3f& scale()
		{
			_is_dirty = true;
			return _scale;
		}

		std::optional<entity_id>& parent_id() 
		{ 
			_is_dirty = true;
			return _parent_id; 
		}

		// hierarchy
		const std::optional<entity_id>& parent_id() const { return _parent_id; }

		// matrices
		const affine_transform& local_to_world() const { return _local_to_world; }
		const affine_transform& local_to_parent() const { return _local_to_parent; }
		
		// trs
		const Eigen::Translation3f& position() const { return _translation; }
		const Eigen::Quaternionf& rotation() const { return _rotation; }
		const Eigen::Vector3f& scale() const { return _scale; }

		bool is_dirty() const { return _is_dirty; }
		void set_clean() { _is_dirty = false; }

	private:		
		affine_transform _local_to_world;
		affine_transform _local_to_parent;
		Eigen::Translation3f _translation;
		Eigen::Quaternionf _rotation;
		Eigen::Vector3f _scale;
		
		bool _is_dirty;

		std::optional<entity_id> _parent_id;
	};
}

#endif