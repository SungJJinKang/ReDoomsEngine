#include "JobSystem.h"

#include "sysinfoapi.h"

void FJobContainer::DoJob()
{
	Job(JobDispatchArgs);
}

static EA::Thread::Futex ThreadInitFutex{};

void FJobSystem::Init()
{
	SYSTEM_INFO SystemInfo{};
	GetSystemInfo(&SystemInfo);
	const uint32_t LogicalProcessorCount = SystemInfo.dwNumberOfProcessors;
	const uint32_t JobThreadCount = LogicalProcessorCount / 2;

	EA::Thread::ThreadPoolParameters ThreadPoolParam{};
	ThreadPoolParam.mnMaxCount = 0;
	ThreadPoolParam.mnInitialCount = 0;
	ThreadPoolParam.mnIdleTimeoutMilliseconds = EA::Thread::kTimeoutNone;
	ThreadPoolParam.mDefaultThreadParameters.mpName = "JobThread";

	ThreadPool = eastl::make_unique<EA::Thread::ThreadPool>(&ThreadPoolParam);

	for (uint32_t JobIndex = 0; JobIndex < JobThreadCount; ++JobIndex)
	{
		EA::Thread::ThreadParameters ThreadParam{};
		ThreadParam.mnPriority = EA::Thread::kProcessorAny;

		eastl::string ThreadName;
		ThreadName.sprintf("JobThread(%d)", JobIndex);
		ThreadParam.mpName = ThreadName.data();

		EA::Thread::MakeThread([ThreadName, ThreadParam, this]() {
			TLSThreadName = ThreadName;
			TLSThreadType = ThreadType::JobThread;

			EA::Thread::AutoFutex f{ ThreadInitFutex };
			{
				EA::Thread::ThreadParameters TempThreadParam = ThreadParam;
				TempThreadParam.mpName = ThreadName.data();

				ThreadPool->AddThread(TempThreadParam, true);
			}
		}, 
		ThreadParam);
	}
}

void FJobSystem::OnStartFrame(FD3D12CommandContext& InCommandContext)
{

}

void FJobSystem::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}

void FJobSystem::ProcessJobsOnCallerThread()
{
	EA::Thread::ThreadParameters ThreadParam{};
	ThreadParam.mnPriority = EA::Thread::kProcessorAny;
	ThreadParam.mpName = TLSThreadName.data();
	ThreadPool->ProcessJobFromCallerThread(ThreadParam);
}

eastl::vector<FJobResult> FJobSystem::Dispatch(const uint32_t InJobCount, const JobType& job)
{
	eastl::vector<FJobResult> JobResults{};
	JobResults.reserve(InJobCount);

	for (uint32_t JobIndex = 0; JobIndex < InJobCount; ++JobIndex)
	{
		FJobContainer* NewJob = new FJobContainer();
		NewJob->Job = job;
		NewJob->JobDispatchArgs.JobIndex = JobIndex;

		const int32_t JobID = ThreadPool->Begin(
			[](void* Job)->intptr_t 
			{
				FJobContainer* JobContainer = reinterpret_cast<FJobContainer*>(Job);
				JobContainer->DoJob();
				delete JobContainer;
				return 0;
			},
			NewJob
		);

		FJobResult JobResult{ this, JobID };
		JobResults.emplace_back(JobResult);
	}

	return JobResults;
}

void FJobSystem::WaitForJobCompletion(const int32_t InJobID, const uint64_t InWaitTimeMilliSeconds /*= EA::Thread::kTimeoutNone*/)
{
	ThreadPool->WaitForJobCompletion(InJobID, EA::Thread::ThreadPool::kJobWaitNone /*meaningless param*/, InWaitTimeMilliSeconds);
}

void FJobSystem::WaitForAllJobCompletion(const uint64_t InWaitTimeMilliSeconds)
{
	ThreadPool->WaitForJobCompletion(-1, EA::Thread::ThreadPool::kJobWaitAll, InWaitTimeMilliSeconds);
}

FJobResult::FJobResult(FJobSystem* const InJobSystem, const int32_t InJobIndex)
	: JobSystem(InJobSystem), JobIndex(InJobIndex)
{

}

void FJobResult::WaitForJobCompletion(const uint64_t InWaitTimeMilliSeconds)
{
	JobSystem->WaitForJobCompletion(JobIndex, InWaitTimeMilliSeconds);
}
