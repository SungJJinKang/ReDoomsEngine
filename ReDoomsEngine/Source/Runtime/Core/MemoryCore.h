#pragma once
#include "new"
#include "EABase/eabase.h"
#include "EAStdC/EAMemory.h"
#include "TextHelper.h"
#include <type_traits>


struct FScopedMemoryTrace
{
	// use address of literal string as key
	FScopedMemoryTrace(const char* const InTraceName);
	~FScopedMemoryTrace();

	const char* const TraceName;
};

void InitMemoryCore();

#define SCOPED_MEMORY_TRACE(TRACE_NAME) FScopedMemoryTrace RD_CONCAT(ScopedMemoryTrace, RD_UNIQUE_NAME(TRACE_NAME)){#TRACE_NAME};

#define MEM_ZERO(VARIABLE) static_assert(!std::is_pointer<decltype (VARIABLE)>::value); EA::StdC::Memset8(&VARIABLE, 0, sizeof(VARIABLE))