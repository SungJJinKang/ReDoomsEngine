#include "MemoryCore.h"
#include <new>
#if USE_MIMALLOC
#include "mimalloc.h"
#endif
#include "EASTL/stack.h"
#include "EASTL/hash_map.h"
#include "EAAssert/eaassert.h"

#if ENABLE_PROFILER
#include "Profiling.h"
extern eastl::vector<const char*> MemoryTraceStack{};

// use literal string address as key
extern eastl::hash_map<const char*, FMemoryTraceData> TraceDataMap{};
#endif

void InitMemoryCore()
{
#if USE_MIMALLOC && RD_DEBUG
	mi_option_enable(mi_option_e::mi_option_show_stats);
	mi_option_enable(mi_option_e::mi_option_show_errors);
	mi_option_enable(mi_option_e::mi_option_verbose);
#endif
}

static void AllocateToMemoryTraceStack(const size_t InSize)
{
	#if ENABLE_PROFILER
	if (MemoryTraceStack.size() > 0)
	{
		TraceDataMap[MemoryTraceStack.back()].Allocated += InSize;
	}
	#endif
}

static void DeallocateFromMemoryTraceStack(const size_t InSize)
{
	#if ENABLE_PROFILER
	if (MemoryTraceStack.size() > 0)
	{
		FMemoryTraceData& TraceData = TraceDataMap[MemoryTraceStack.back()];
		TraceData.Allocated -= InSize;
		EA_ASSERT(TraceData.Allocated >= 0);
	}
	#endif
}

// this is for eastl
void* operator new(size_t size, const char* pName, int flags, unsigned int debugFlags, const char* file, int line)
{
	#if USE_MIMALLOC
	AllocateToMemoryTraceStack(size);
	return mi_malloc(size);
	#else
	return new char[size];
	#endif
}

void* operator new[](size_t size, const char* pName, int flags, unsigned int debugFlags, const char* file, int line)
{ 
	#if USE_MIMALLOC
	AllocateToMemoryTraceStack(size);  
	return mi_malloc(size); 
	#else
	return new char[size];
	#endif
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	#if USE_MIMALLOC
	AllocateToMemoryTraceStack(size);
	return mi_malloc_aligned(size, alignment); 
	#else
	return new char[size];
	#endif
}

#if USE_MIMALLOC
#if defined(_MSC_VER) && defined(_Ret_notnull_) && defined(_Post_writable_byte_size_)
// stay consistent with VCRT definitions
#define mi_decl_new(n)          mi_decl_nodiscard mi_decl_restrict _Ret_notnull_ _Post_writable_byte_size_(n)
#define mi_decl_new_nothrow(n)  mi_decl_nodiscard mi_decl_restrict _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(n)
#else
#define mi_decl_new(n)          mi_decl_nodiscard mi_decl_restrict
#define mi_decl_new_nothrow(n)  mi_decl_nodiscard mi_decl_restrict
#endif

void operator delete(void* p) noexcept 
{ 
	mi_free(p); 
};
void operator delete[](void* p) noexcept 
{
	mi_free(p); 
};

void operator delete  (void* p, const std::nothrow_t&) noexcept
{
	mi_free(p);
}
void operator delete[](void* p, const std::nothrow_t&) noexcept
{ 
	mi_free(p);
}

mi_decl_new(n) void* operator new(std::size_t n) noexcept(false)
{
	AllocateToMemoryTraceStack(n);
	return mi_new(n);
}
mi_decl_new(n) void* operator new[](std::size_t n) noexcept(false) 
{
	AllocateToMemoryTraceStack(n);
	return mi_new(n);
}

mi_decl_new_nothrow(n) void* operator new  (std::size_t n, const std::nothrow_t& tag) noexcept 
{
	AllocateToMemoryTraceStack(n);
	(void)(tag); return mi_new_nothrow(n); 
}
mi_decl_new_nothrow(n) void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept 
{
	AllocateToMemoryTraceStack(n);
	(void)(tag); return mi_new_nothrow(n); 
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)
void operator delete  (void* p, std::size_t n) noexcept 
{
	mi_free_size(p, n); 
};
void operator delete[](void* p, std::size_t n) noexcept 
{
	mi_free_size(p, n);
};
#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))
void operator delete  (void* p, std::align_val_t al) noexcept 
{
	mi_free_aligned(p, static_cast<size_t>(al)); 
}
void operator delete[](void* p, std::align_val_t al) noexcept 
{ 
	mi_free_aligned(p, static_cast<size_t>(al));
}
void operator delete  (void* p, std::size_t n, std::align_val_t al) noexcept 
{ 
	mi_free_size_aligned(p, n, static_cast<size_t>(al));
};
void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept
{
	mi_free_size_aligned(p, n, static_cast<size_t>(al)); 
};
void operator delete  (void* p, std::align_val_t al, const std::nothrow_t&) noexcept 
{ 
	mi_free_aligned(p, static_cast<size_t>(al)); 
}
void operator delete[](void* p, std::align_val_t al, const std::nothrow_t&) noexcept 
{ 
	mi_free_aligned(p, static_cast<size_t>(al)); 
}

void* operator new  (std::size_t n, std::align_val_t al) noexcept(false) 
{
	AllocateToMemoryTraceStack(n);
	return mi_new_aligned(n, static_cast<size_t>(al));
}
void* operator new[](std::size_t n, std::align_val_t al) noexcept(false) 
{
	AllocateToMemoryTraceStack(n);
	return mi_new_aligned(n, static_cast<size_t>(al)); 
}
void* operator new  (std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept 
{
	AllocateToMemoryTraceStack(n);
	return mi_new_aligned_nothrow(n, static_cast<size_t>(al));
}
void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept
{
	AllocateToMemoryTraceStack(n);
	return mi_new_aligned_nothrow(n, static_cast<size_t>(al)); 
}
#endif

#endif
