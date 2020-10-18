#include <renderer/skeletal_animation.hpp>
#include <math/interpolate.hpp>


Eigen::Matrix4f renderer::skeletal_animation::eval(animation_time time)
{
	return Eigen::Matrix4f::Identity();

	if (duration == animation_time::zero())
	{
		return Eigen::Matrix4f::Identity();
	}

	if (time == animation_time::zero())
	{		
		Eigen::Matrix4f t = Eigen::Matrix4f::Identity();
		t.col(3).head<3>() = position.values[0];
		Eigen::Matrix4f r = rotation.values[0].matrix();		
		return t * r * Eigen::Affine3f(Eigen::Scaling(scale.values[0])).matrix();
	}
	
	std::uint16_t pos_ind{ 0 };
	while (time > position.times[pos_ind] && pos_ind < position.frame_count)
		pos_ind++;

	std::uint16_t rotation_ind{ 0 };
	while (time > rotation.times[rotation_ind] && rotation_ind < rotation.frame_count)
		rotation_ind++;

	std::uint16_t scale_ind{ 0 };
	while (time > scale.times[scale_ind] && scale_ind < scale.frame_count)
		scale_ind++;

	///// skip interpolation for debugging
//    glm::mat4 t = glm::translate(glm::mat4(1.f), position.values[pos_ind]);
//    glm::mat4 r = glm::mat4_cast(rotation.values[rotation_ind]);
//    glm::mat4 s = glm::scale(glm::mat4(1.f), scale.values[scale_ind]);
//    return t * r * s;
	Eigen::Matrix4f t = Eigen::Affine3f(Eigen::Translation3f(position.values[pos_ind])).matrix();	
	Eigen::Matrix4f r = rotation.values[rotation_ind].matrix();
	Eigen::Matrix4f s = Eigen::Affine3f(Eigen::Scaling(scale.values[scale_ind])).matrix();
	return t * r * s;
	/////

	//auto t_range = position.times[pos_ind + 1] - position.times[pos_ind];
	//float pos_interp_t = (time - position.times[pos_ind]) / t_range;
	//Eigen::Translation3f interpolated_pos(math::lerp(
	//	position.values[scale_ind], 
	//	position.values[scale_ind + 1], 
	//	pos_interp_t));
	//
	//t_range = rotation.times[rotation_ind + 1] - rotation.times[rotation_ind];
	//float rot_interp_t = (time - rotation.times[rotation_ind]) / t_range;
	//math::quat<float> interpolated_rot = 
	//	rotation.values[rotation_ind].slerp(
	//	rotation.values[rotation_ind + 1], 
	//	rot_interp_t);
	//
	//t_range = scale.times[scale_ind + 1] - scale.times[scale_ind];
	//auto scl_interp_t = (time - scale.times[scale_ind]) / t_range;
	//Eigen::Vector3f interpolated_scale = math::lerp_scale(
	//	scale.values[scale_ind], 
	//	scale.values[scale_ind + 1], 
	//	scl_interp_t);
	//
	//Eigen::Matrix4f t = Eigen::Affine3f(interpolated_pos).matrix();
	//Eigen::Matrix4f r = interpolated_rot.matrix();
	//Eigen::Matrix4f s = Eigen::Affine3f(Eigen::Scaling(interpolated_scale)).matrix(); 
	//return t * r * s;
}
