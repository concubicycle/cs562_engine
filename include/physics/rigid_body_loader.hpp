#ifndef __RB_LOADER_HPP_
#define __RB_LOADER_HPP_
#include "asset/component_loader.hpp"
#include "rigid_body.hpp"

namespace physics
{

  class rigid_body_loader : public asset::component_loader
  {
  public:
    void load(asset::asset_loader_node& node) override;
    component_bitset components_to_load() override;
  };
}

#endif