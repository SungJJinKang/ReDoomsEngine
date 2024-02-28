#pragma once

#include "EASTL/string.h"
#include "Enums.h"

extern thread_local eastl::string TLSThreadName;
extern thread_local ThreadType TLSThreadType;