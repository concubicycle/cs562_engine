#ifndef __LOCAL_PUNCTUAL_LIGHT_HPP_
#define __LOCAL_PUNCTUAL_LIGHT_HPP_

#include <Eigen/Core>
#include <ecs/component.hpp>

namespace renderer
{
	struct local_punctual_light : public ecs::component<local_punctual_light>
	{
		Eigen::Array3f color;

		void set_radius(float val)
		{
			_radius_dirty = true;
			_radius = val;
		}

		float radius() const { return _radius; }
		bool radius_dirty() const { return _radius_dirty; }

		void set_clean()
		{
			_radius_dirty = false;
		}

	private:
		float _radius;
		bool _radius_dirty;
	};
}


#endif