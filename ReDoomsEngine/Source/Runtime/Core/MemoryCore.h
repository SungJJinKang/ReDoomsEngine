#pragma once
#include "new"
#include "EABase/eabase.h"
#include "EAStdC/EAMemory.h"
#include <type_traits>

void* operator new     (size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new     (size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);

void  operator delete  (void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete[](void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete  (void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete[](void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);

#define MEM_ZERO(VARIABLE) static_assert(!std::is_pointer<decltype (VARIABLE)>::value); EA::StdC::Memset8(&VARIABLE, 0, sizeof(VARIABLE))