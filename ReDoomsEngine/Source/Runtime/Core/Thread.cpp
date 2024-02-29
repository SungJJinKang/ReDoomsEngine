#include "Thread.h"

thread_local eastl::string TLSThreadName = "Unknown";
thread_local EThreadType TLSThreadType = EThreadType::Unknown;

bool IsOnMainThread()
{
	return TLSThreadType == EThreadType::MainThread;
}

bool IsOnRHIThread()
{
	return TLSThreadType == EThreadType::RHIThread;
}

bool IsOnJobThread()
{
	return TLSThreadType == EThreadType::JobThread;
}
