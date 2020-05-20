// Largely from Game Engine Architecture Third Ed.
// Gregory, Jason.Game Engine Architecture, Third Edition(p. 432).CRC Press.Kindle Edition.

#ifndef __ALIGNMENT_HPP_
#define __ALIGNMENT_HPP_

#include <cstdint>
#include <stddef.h>

namespace memory
{
	// Shift the given address upwards if/as necessary to  // ensure it is aligned to the given number of bytes.  
	uintptr_t align_address(uintptr_t addr, size_t align);

	// Aligned allocation function. IMPORTANT: 'align'  // must be a power of 2 (typically 4, 8 or 16).  
	void* alloc_aligned(size_t bytes, size_t align);	

	// Shift the given pointer upwards if/as necessary to  // ensure it is aligned to the given number of bytes.  
	template<typename T>
	T* align_pointer(T* ptr, size_t align)
	{
		const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
		const uintptr_t addrAligned = align_address(addr, align);
		return reinterpret_cast<T*>(addrAligned);
	}
}

#endif // !__ALIGNMENT_HPP_
