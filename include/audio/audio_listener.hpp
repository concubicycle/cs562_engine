//
// Created by sava on 1/27/20.
//

#ifndef _AUDIO_LISTENER_HPP
#define _AUDIO_LISTENER_HPP

#include <ecs/component.hpp>
#include <Eigen/Core>

#include <atomic>

namespace audio
{
    struct audio_listener : public ecs::component<audio_listener>
    {        
        int listener_id {0};

        Eigen::Vector3f listener_position;
        Eigen::Vector3f listener_velocity;
        Eigen::Vector3f listener_forward;
        Eigen::Vector3f listener_up;
    };
}

#endif //WIZARDENGINE_AUDIO_LISTENER_HPP
