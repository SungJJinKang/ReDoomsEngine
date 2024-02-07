#include "MemoryCore.h"
#include <new>
#include "mimalloc.h"
#include "EASTL/stack.h"
#include "EASTL/hash_map.h"
#include "EAAssert/eaassert.h"

struct FMemoryTraceData
{
	const char* TraceName = nullptr;
	uint64_t Allocated = 0;
};

static eastl::vector<const char*> MemoryTraceStack{};

// use literal string address as key
static eastl::hash_map<const char*, FMemoryTraceData> TraceDataMap{};

FScopedMemoryTrace::FScopedMemoryTrace(const char* const InTraceName)
	: TraceName(InTraceName)
{
	FMemoryTraceData& TraceData = TraceDataMap.try_emplace(InTraceName).first->second;
	TraceData.TraceName = InTraceName;

	if (MemoryTraceStack.size() > 0)
	{
		EA_ASSERT(MemoryTraceStack.back() != InTraceName);
	}
	MemoryTraceStack.emplace_back(InTraceName);
}

FScopedMemoryTrace::~FScopedMemoryTrace()
{
	EA_ASSERT(MemoryTraceStack.back() == TraceName);
	MemoryTraceStack.pop_back();
}

void InitMemoryCore()
{
	mi_option_enable(mi_option_e::mi_option_show_stats);
	mi_option_enable(mi_option_e::mi_option_show_errors);
	mi_option_enable(mi_option_e::mi_option_verbose);
}

static void AllocateToMemoryTraceStack(const size_t InSize)
{
	if (MemoryTraceStack.size() > 0)
	{
		TraceDataMap[MemoryTraceStack.back()].Allocated += InSize;
	}
}

static void DeallocateFromMemoryTraceStack(const size_t InSize)
{
	if (MemoryTraceStack.size() > 0)
	{
		FMemoryTraceData& TraceData = TraceDataMap[MemoryTraceStack.back()];
		TraceData.Allocated -= InSize;
		EA_ASSERT(TraceData.Allocated >= 0);
	}
}

// this is for eastl
void* operator new[](size_t size, const char* pName, int flags, unsigned     debugFlags, const char* file, int line)
{ 
	AllocateToMemoryTraceStack(size);  
	return mi_malloc(size); 
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	AllocateToMemoryTraceStack(size);
	return mi_malloc_aligned(size, alignment); 
}

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
