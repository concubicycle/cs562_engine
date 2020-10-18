#include <renderer/skeleton_joint.hpp>

void renderer::skeleton_joint::compute_transform(
	Eigen::Matrix4f& global_inverse,
	const Eigen::Matrix4f& parent_transform,
	animation_time t,
	std::uint16_t animation_index,
	Eigen::Matrix4f* buffer)
{
	/*Eigen::Matrix4f& calculated_transform = buffer[bone_id];
	auto& animation = animations[animation_index];

	Eigen::Matrix4f node_transform = animation.is_blank()
		? base_transform
		: animation.eval(t);

	Eigen::Matrix4f global_transform = parent_transform * node_transform;
	calculated_transform = global_transform * offset;

	for (std::uint16_t i = 0; i < child_count; ++i)
		children[i]->compute_transform(global_inverse, global_transform, t, animation_index, buffer);*/


}


