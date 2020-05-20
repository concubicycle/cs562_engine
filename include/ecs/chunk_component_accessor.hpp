//
// Created by sava on 10/8/19.
//

#ifndef __CHUNK_COMPONENT_ACCESSOR_H_
#define __CHUNK_COMPONENT_ACCESSOR_H_

#include <map>
#include <ecs/archetype_chunk_component.hpp>
#include <memory/alignment.hpp>


namespace ecs
{
    struct component_ptr
    {
        std::uint32_t component_offset; // from chunk_ptr
        std::uint8_t component_bitshift;
    };

    // a chunk will store first an array of component_ptr structs, and
    // then the components themselves. this header pointer points to the
    // array of component_ptr structs, which contain means of getting
    // the actual component pointers.
    struct chunk_description
    {
        size_t chunk_size;
        uintptr_t header_offset;
        uintptr_t components_offset;
        std::uint16_t component_count;
    };


    class chunk_component_accessor
    {
        // chunk padding in case of overflow
        // should ideally be 0
        static const std::uintptr_t PaddingHack = 16; 

    public:
        chunk_component_accessor(void *chunk_ptr, component_bitset archetype_id);

        void *ptr() const { return _chunk_ptr; }

        /**
        * The component metadata objects stored in archetype_chunk_component.meta contain wrappers for
        * in-place constructor and destructor of that particular type. We can go through all the
        * archetype_chunk_component instances, grab pointer offset, and construct/destruct where it says.
        */
        void construct();

        void destruct();

        template<class T>
        T *get_component()
        {
            return (T *) get_component(component<T>::component_bitshift);
        }

        void* get_component(std::uint8_t component_bitshift);


        /**
        * Calculate the chunk size for an archetype chunk.
        * The whole chunk has to be aligned like the component_ptr, since it will be
        * prefixed by an array of component_ptrs.
         *
        * After the first component, add alignment padding for each subsequent component.
        *
        * @param archetype_id: the set of components for which to calculate chunk size
        *
        * @return a description of various offsets in the chunk for this archetype
        */
        static chunk_description calculate_chunk_description(component_bitset archetype_id, void* chunk_ptr);
        static chunk_description calculate_worst_case_chunk_description(component_bitset archetype_id);

        static size_t chunk_align() { return alignof(component_ptr); }


    private:
        void *_chunk_ptr;
        chunk_description _description;
        component_ptr* _header;

        component_ptr* header(void* chunk_ptr, uintptr_t header_offset);
    };

} // namespace ecs

#endif //__CHUNK_COMPONENT_ACCESSOR_H_
