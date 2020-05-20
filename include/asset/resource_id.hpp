//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_RESOURCE_ID_HPP
#define WIZARDENGINE_RESOURCE_ID_HPP

#include <atomic>
#include <cstdint>

namespace asset
{
    typedef std::uint32_t resource_id;

    resource_id next_resource_id();
}


#endif //WIZARDENGINE_RESOURCE_ID_HPP
