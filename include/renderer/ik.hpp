#ifndef __IK_HPP_
#define __IK_HPP_

#include <Eigen/Core>
#include <transforms/transform.hpp>
#include <vector>
#include "skeleton_structs.hpp"

namespace renderer
{
  struct ccd_chain_link
  {
    joint* joint;

    ccd_chain_link(renderer::joint* j) : joint(j) {}
  };


  class ik_ccd
  {    
    float DefaultThreshold = 0.1f;
    float DefaultSmartBounceTolerance = 0.001f;
    float DefaultDuration = 1.f;
    float Epsilon = 0.0000001f;
    std::uint32_t DefaultMaxIterations = 32;    

  public:
    ik_ccd(std::string name, skeleton* skeleton, transforms::transform* transform);

    [[nodiscard]] Eigen::Vector3f get_target() const;
    void set_target(const Eigen::Vector3f& t);
    float* get_target_data();

    void fill_link_sequence_ftb(size_t offset, size_t count);
    void fill_link_sequence_btf(size_t offset, size_t count);

    void fill_link_sequence_ftb(size_t offset);
    void fill_link_sequence_btf(size_t offset);

    void update(float dt);
    void step();
    void toggle();
    void reset();

    skeleton_pose& result_pose();

    bool _smart_bounce{ false };

  private:    
    skeleton* _skeleton;
    joint* _end_effector;
    skeleton_pose _result_pose;
    Eigen::Vector4f _ee_start;
    transforms::transform* _transform;

    std::vector<ccd_chain_link> _pose_chain{};
    std::vector<ccd_chain_link*> _link_sequence{};
    Eigen::Vector3f _target{ 1, 1, 1 };

    float _threshold{ DefaultThreshold };
    float _thresholdSq{ DefaultThreshold * DefaultThreshold };
    float _smart_bounce_tolerance{ DefaultSmartBounceTolerance };
    float _duration{ DefaultDuration };
    float _elapsed{ 0 };
    std::uint32_t _max_iterations{ DefaultMaxIterations };    

    size_t _step_index{ 0 };
    bool _enabled{ false };
    
    Eigen::Vector4f _animation_space_target;


    joint* find_end_effector(std::string joint_name);
    float step_internal();
  };
}

#endif
