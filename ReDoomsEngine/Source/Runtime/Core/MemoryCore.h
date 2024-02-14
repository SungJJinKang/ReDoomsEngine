#pragma once
#include "new"
#include "EABase/eabase.h"
#include "EAStdC/EAMemory.h"
#include "TextHelper.h"
#include <type_traits>

#define USE_MIMALLOC 1 // when tesed on shipping build, mimalloc is 5~10% faster than default allocator

void InitMemoryCore();

#define MEM_ONE(VARIABLE) static_assert(!std::is_pointer<decltype (VARIABLE)>::value); EA::StdC::Memset8(&VARIABLE, 0xFF, sizeof(VARIABLE))
#define MEM_ZERO(VARIABLE) static_assert(!std::is_pointer<decltype (VARIABLE)>::value); EA::StdC::Memset8(&VARIABLE, 0, sizeof(VARIABLE))