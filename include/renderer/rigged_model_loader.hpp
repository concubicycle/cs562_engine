#ifndef __RIGGED_MODEL_LOADER_HPP_
#define __RIGGED_MODEL_LOADER_HPP_

#include <Eigen/Geometry>
#include <Eigen/Core>

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <util/string_table.hpp>
#include <renderer/assimp_vram_loader.hpp>
#include <renderer/rigged_model_instance.hpp>

namespace renderer
{
	class rigged_model_loader : public asset::component_loader
	{
	public:
		rigged_model_loader(
			util::string_table& strings,
			asset::asset_loader& asset_loader,
			assimp_vram_loader& vram_loader);

		virtual void load(asset::asset_loader_node& ecs_node) override;
		virtual component_bitset components_to_load() override;

	private:
		util::string_table& _strings;
		asset::asset_loader& _asset_loader;
		assimp_vram_loader& _vram_loader;	

		[[nodiscard]] animation_time tick_to_time(double ticks_per_second, double ticks) const;
		[[nodiscard]] Eigen::Matrix4f map_matrix(const aiMatrix4x4& mat) const;
		[[nodiscard]] Eigen::Vector3f map_vec3(aiVector3D& vec) const;
		[[nodiscard]] math::quat<float> map_quat(aiQuaternion q) const;
	};
}


#endif