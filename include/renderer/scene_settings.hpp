#ifndef __PARTICIPATING_MEDIUM_HPP_
#define __PARTICIPATING_MEDIUM_HPP_

#include <ecs/component.hpp>
#include <cstdint>
#include <Eigen/Core>

namespace renderer
{
	struct participating_medium_settings
	{
		float beta{ 0.04f };
		float f_lookup_range{ 10.f };
		Eigen::Vector3f initial_intensity{ 50000.f, 50000.f, 50000.f };
		bool use_single_scattering{ false };
		float light_depth_scale{ 1.0f };
	};

	struct ambient_occlusion_settings
	{
		float range_of_influence{ 3.f };
		float c_coefficient{ 0.1f };
		std::uint32_t n{ 20 };
		float sigma{ 0.001f };
		float scale{ 1.f };
		float contrast { 1.f };
		bool draw_only_ao { false };
	};

	struct animation_settings
	{
		bool draw_bones{ false };
	};

	struct lighting_settings
	{
		bool ibl{ false };
	};

	struct scene_settings : public ecs::component<scene_settings>
	{
		participating_medium_settings participating_medium;
		ambient_occlusion_settings ambient_occlusion;
		animation_settings animation;
		lighting_settings light;
	};
}


#endif