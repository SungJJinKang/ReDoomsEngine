#include "MemoryCore.h"
#include "mimalloc.h"
#include "mimalloc-new-delete.h"
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

void* operator new[](size_t size, const char* pName, int flags, unsigned     debugFlags, const char* file, int line)
{ return mi_malloc(size); }
	void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{ return mi_malloc_aligned(size, alignment); }