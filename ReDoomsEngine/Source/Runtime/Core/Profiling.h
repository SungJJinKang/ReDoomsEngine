#pragma once

#include "Macros.h"

#define ENABLE_PROFILER 0
//#define ENABLE_PROFILER RD_DEBUG

#include "EASTL/vector_map.h"
#if ENABLE_PROFILER
#include "EASTL/array.h"
#include "EASTL/vector.h"
#endif

class FD3D12CommandQueue;
class FD3D12CommandList;
struct FD3D12CommandContext;

struct FCPUTimer
{
public:

	struct FNode
	{
		FNode(const char* const InTimerName);

		void RecursiveClearElapsedSeconds();
		void RecursiveCalculateAverage(const bool bUpdateShownAverage);
		void RecursiveDestory();

		const char* const TimerName;

		double ElapsedSeconds;
		double Average = 0.0;
		double ShownAverage = 0.0;

		eastl::vector_map<const char*, FCPUTimer::FNode*> Childs;
	};

	FCPUTimer(const char* const InTimerName, const bool bInScopedTimer = true);
	~FCPUTimer();

	void Start();
	void End();

	inline const char* const GetTimerName() const { return TimerName; }

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

	const bool bScopedTimer;
};

#if ENABLE_PROFILER
struct FScopedMemoryTrace
{
	// use address of literal string as key
	FScopedMemoryTrace(const char* const InTraceName);
	~FScopedMemoryTrace();

	const char* const TraceName;
};

struct FMemoryTraceData
{
	const char* TraceName = nullptr;
	uint64_t Allocated = 0;
};
#endif

struct FGPUTimer
{
public:

	#if ENABLE_PROFILER
	FGPUTimer(const char* const InTimerName);
	FGPUTimer(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList, const char* const InTimerName);
	~FGPUTimer();
	void Start(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList);
	void End(FD3D12CommandList* const InCommandList);
	#else
	inline FGPUTimer(const char* const InTimerName) {}
	inline FGPUTimer(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList, const char* const InTimerName) {}
	inline ~FGPUTimer() {}
	inline void Start(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList) {}
	inline void End(FD3D12CommandList* const InCommandList) {}

#endif

private:

	#if ENABLE_PROFILER
	FD3D12CommandList* TargetCommandList;
	const char* TimerName;
	bool bScoped;
	#endif
};

#if ENABLE_PROFILER
void CPUTimerBeginFrame();
void CPUTimerEndFrame();
void GPUTimerBeginFrame(FD3D12CommandContext* const InD3D12CommandContext);
void GPUTimerEndFrame(FD3D12CommandContext* const InD3D12CommandContext);
void DestroyTimerData();
#else
inline void CPUTimerBeginFrame() {}
inline void CPUTimerEndFrame() {}
inline void GPUTimerBeginFrame(FD3D12CommandContext* const InD3D12CommandContext) {}
inline void GPUTimerEndFrame(FD3D12CommandContext* const InD3D12CommandContext) {}
inline void DestroyTimerData() {}
#endif
#if ENABLE_PROFILER

#define SCOPED_CPU_TIMER(TRACE_NAME) FCPUTimer RD_CONCAT(FCPUTimer, RD_UNIQUE_NAME(TRACE_NAME)){#TRACE_NAME};
#define SCOPED_GPU_TIMER_DIRECT_QUEUE(COMMAND_CONTEXT, TRACE_NAME) FGPUTimer RD_CONCAT(FGPUTimer, RD_UNIQUE_NAME(TRACE_NAME)){COMMAND_CONTEXT.CommandQueueList[ED3D12QueueType::Direct], COMMAND_CONTEXT.GraphicsCommandList.get(), #TRACE_NAME};
#define SCOPED_GPU_TIMER_DIRECT_QUEUE_COMMAND_LIST(COMMAND_CONTEXT, COMMAND_LIST, TRACE_NAME) FGPUTimer RD_CONCAT(FGPUTimer, RD_UNIQUE_NAME(TRACE_NAME)){COMMAND_CONTEXT.CommandQueueList[ED3D12QueueType::Direct], COMMAND_LIST, #TRACE_NAME};
#define SCOPED_MEMORY_TRACE(TRACE_NAME) FScopedMemoryTrace RD_CONCAT(ScopedMemoryTrace, RD_UNIQUE_NAME(TRACE_NAME)){#TRACE_NAME};

#else

#define SCOPED_CPU_TIMER(TRACE_NAME)
#define SCOPED_GPU_TIMER_DIRECT_QUEUE(COMMAND_CONTEXT, TRACE_NAME)
#define SCOPED_GPU_TIMER_DIRECT_QUEUE_COMMAND_LIST(COMMAND_CONTEXT, COMMAND_LIST, TRACE_NAME)
#define SCOPED_MEMORY_TRACE(TRACE_NAME)

#endif
