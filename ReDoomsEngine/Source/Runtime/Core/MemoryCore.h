#pragma once
#include "new"
#include "EABase/eabase.h"

void* operator new     (size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new     (size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);

void  operator delete  (void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete[](void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete  (void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void  operator delete[](void* p, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line);

