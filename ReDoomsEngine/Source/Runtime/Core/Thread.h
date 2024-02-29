#pragma once

#include "EASTL/string.h"
#include "Enums.h"

extern thread_local eastl::string TLSThreadName;
extern thread_local EThreadType TLSThreadType;

bool IsOnMainThread();
bool IsOnRHIThread();
bool IsOnJobThread();