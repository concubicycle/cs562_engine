#ifndef __COMPONENT_LOADER_HPP_
#define __COMPONENT_LOADER_HPP_

#include <asset/asset_loader_node.hpp>
#include <asset/scene_entity.hpp>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace asset
{
  class component_loader
  {
  public:
    virtual ~component_loader() {}

    virtual void load(asset_loader_node& ecs_node) = 0;
    virtual component_bitset components_to_load() = 0;

  protected:
    static const component_bitset Anything = 0;
  };
}

#endif // !__COMPONENT_LOADER_HPP_
