//
// Created by sava on 10/8/19.
//

#ifndef __COMPONENT_H_
#define __COMPONENT_H_

#include <string>

#include "ecs_types.hpp"


namespace ecs
{
    template<class T>
    class component
    {
    public:
        static const component_shift component_bitshift;
        static const component_bitset archetype_bit;
    };

    template<class T>
    const component_bitset component<T>::archetype_bit = component_bitset(1) << T::component_bitshift;
}

#endif //__COMPONENT_H_
