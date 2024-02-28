#pragma once
#include "CommonInclude.h"
#include "Common/RendererStateCallbackInterface.h"

#include "eathread/eathread_pool.h"

struct FJobDispatchArgs;
class FJobSystem;
using JobType = eastl::function<void(FJobDispatchArgs)>;

struct FJobDispatchArgs
{
	uint32_t JobIndex;
};

struct FJobContainer
{
	JobType Job;
	FJobDispatchArgs JobDispatchArgs;

	void DoJob();
};

class FJobResult
{
public:
	
	FJobResult(FJobSystem* const InJobSystem, const int32_t InJobIndex);
	void WaitForJobCompletion(const uint64_t InWaitTimeMilliSeconds = EA::Thread::kTimeoutNone);

private:

	FJobSystem* const JobSystem;
	int32_t JobIndex;
};

/// <summary>
/// ref : https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/
/// </summary>
class FJobSystem : public EA::StdC::Singleton<FJobSystem>, public IRendererStateCallbackInterface
{
public:

	void Init();
	void OnStartFrame(FD3D12CommandContext& InCommandContext) override;
	void OnEndFrame(FD3D12CommandContext& InCommandContext) override;

	void ProcessJobsOnCallerThread();
	eastl::vector<FJobResult> Dispatch(const uint32_t InJobCount, const JobType& job);
	void WaitForJobCompletion(const int32_t InJobID, const uint64_t InWaitTimeMilliSeconds = EA::Thread::kTimeoutNone);
	void WaitForAllJobCompletion(const uint64_t InWaitTimeMilliSeconds = EA::Thread::kTimeoutNone);

private:

	eastl::unique_ptr<EA::Thread::ThreadPool> ThreadPool;

	eastl::queue<FJobContainer> JobQueue;
	EA::Thread::Mutex JobQueueMutex;
	eastl::vector<EA::Thread::Thread> JobThreadList;
};

