#ifndef __PHYS_COMP_HPP_
#define __PHYS_COMP_HPP_

#include "ecs/ecs_types.hpp"

namespace physics
{
  enum class component_bits : component_shift
  {
    rigid_body = 25
  };
}

#endif