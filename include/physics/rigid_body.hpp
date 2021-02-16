#ifndef __RIGID_BODY_HPP_
#define __RIGID_BODY_HPP_

#include <optional>
#include <ecs/component.hpp>
#include "model.hpp"

namespace physics
{
  struct rigid_body : public ecs::component<rigid_body>
  {
    std::optional<model> model;
    float gravity { 10.f };
  };
}

#endif
