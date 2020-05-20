//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_LISTENER_HPP
#define WIZARDENGINE_AUDIO_LISTENER_HPP

#include <ecs/component.hpp>

#include <glm/vec3.hpp>

#include <atomic>

namespace audio
{
    struct audio_listener : public ecs::component<audio_listener>
    {        
        int listener_id {0};

        glm::vec3 listener_position;
        glm::vec3 listener_velocity;
        glm::vec3 listener_forward;
        glm::vec3 listener_up;
    };
}

#endif //WIZARDENGINE_AUDIO_LISTENER_HPP
