#pragma once

#include "Macros.h"
#include "EASTL/hash_map.h"

struct FCPUTimer;

class FProfilingManager
{
public:
	static void NewFrame();
	static void UpdateCPUTimer(const FCPUTimer* const InTimer);
	static const eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/>& GetCPUTimerElapsedSecondsMap(const uint64_t InFrameIndex);

private:
	static eastl::hash_map<const char* /*Timer name. Literal string*/, double /*Elapsed Seconds*/> CPUTimerElapsedSecondsMap[2];
};

struct FCPUTimer
{
public:

	FCPUTimer(const char* const InTimerName);
	~FCPUTimer();

	void UpdateElapsedTicks();

	inline const char* const GetTimerName() const {	return TimerName; }

	// Get elapsed time since the previous Update call.
	inline unsigned long long GetElapsedTicks() const { return ElapsedTicks; }
	inline double GetElapsedSeconds() const { return ElapsedSeconds; }

	// Integer format represents time using 10,000,000 ticks per second.
	static const unsigned long long TicksPerSecond = 10000000;

	inline static double TicksToSeconds(unsigned long long ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	inline static unsigned long long SecondsToTicks(double seconds) { return static_cast<unsigned long long>(seconds * TicksPerSecond); }

private:
	const char* const TimerName;

	long long QPCLastTime;

	// Derived timing data uses a canonical tick format.
	double ElapsedSeconds;
	unsigned long long ElapsedTicks;
};

#if RD_DEBUG

#define SCOPED_CPU_TIMER(TRACE_NAME) FCPUTimer RD_CONCAT(FCPUTimer, RD_UNIQUE_NAME(TRACE_NAME)){#TRACE_NAME};

#else

#define SCOPED_CPU_TIMER(TRACE_NAME)

#endif