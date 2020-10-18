//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
#define WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP

#include <cstdint>
#include <renderer/animation_time.hpp>

namespace renderer
{
    template <typename T>
    struct keyframe
    {
      keyframe(animation_time t, T v)
        : time(t)
        , value(v) {}

      animation_time time;
      T value;      
    };

    template <typename T, std::uint32_t max_frames>
    struct keyframes
    {
        T values[max_frames];
        animation_time times[max_frames];
        std::uint16_t frame_count{ 0 };
    };

    template <typename T>
    struct property_clip
    {
      std::vector<keyframe<T>> keyframes;
      T identity;
      
      property_clip(T idt) : identity(idt) {}

      inline animation_time end_time() const
      {
        return keyframes.back().time;
      }

      inline const T& end_value() const
      {
        return keyframes.back().value;
      }
    };
}

#endif //WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
