#include "Thread.h"

thread_local eastl::string TLSThreadName = "Unknown";
thread_local ThreadType TLSThreadType = ThreadType::Unknown;
