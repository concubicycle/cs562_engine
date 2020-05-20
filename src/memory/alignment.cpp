#include <memory/alignment.hpp>

#include <cassert>

// Shift the given address upwards if/as necessary to  // ensure it is aligned to the given number of bytes.  
uintptr_t memory::align_address(uintptr_t addr, size_t align)
{
	const size_t mask = align - 1;
	assert((align & mask) == 0); // align is pwr of 2  
	return (addr + mask) & ~mask;
}

// Aligned allocation function. IMPORTANT: 'align'  // must be a power of 2 (typically 4, 8 or 16).  
void* memory::alloc_aligned(size_t bytes, size_t align)
{  
	// Determine worst case number of bytes we'll need.  
	size_t worstCaseBytes = bytes + align - 1;  

	// Allocate unaligned block.  
	std::uint8_t* pRawMem = new std::uint8_t[worstCaseBytes];

	// Align the block.  
	return align_pointer(pRawMem, align);  
}

