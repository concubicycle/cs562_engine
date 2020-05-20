//
// Created by sava on 10/21/19.
//

#ifndef __SYSTEM_INFO_HPP_
#define __SYSTEM_INFO_HPP_

#include <cstdint>

namespace core
{

    class system_info
    {
    public:
        system_info();

        std::uint32_t monitor_width() const { return _monitor_width; }
        std::uint32_t monitor_height() const { return _monitor_height; };

    private:
        std::uint32_t _monitor_width;
        std::uint32_t _monitor_height;

    };

}

#endif //__SYSTEM_INFO_HPP_
