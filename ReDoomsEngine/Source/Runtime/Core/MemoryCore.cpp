#include "MemoryCore.h"
#include "mimalloc.h"
#include "mimalloc-new-delete.h"

void* operator new(size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_new_aligned(size, alignof(std::max_align_t));
}
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_new_aligned(size, alignof(std::max_align_t));
}
void* operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_malloc_aligned_at(size, alignment, alignmentOffset);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_malloc_aligned_at(size, alignment, alignmentOffset);
}

void  operator delete(void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	mi_free_aligned(p, alignof(std::max_align_t));
}
void  operator delete[](void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	mi_free_aligned(p, alignof(std::max_align_t));
}
void  operator delete(void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	mi_free_aligned(p, alignment);
}
void  operator delete[](void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	mi_free_aligned(p, alignment);
}

