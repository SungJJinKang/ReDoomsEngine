#include "JobSystem.h"

#include "sysinfoapi.h"

void FJobContainer::DoJob()
{
	Job(JobDispatchArgs);
}

static EA::Thread::Futex ThreadInitFutex{};

static TConsoleVariable<uint32_t> GJobThreadAffinityMask{ "r.JobThreadAffinityMask", 0xFFFFFFFF };

struct FTestStructure
{
	volatile uint32_t Data1{ 0 };
	uint8_t CachelinePadding[64]{}; // to prevent false sharing
	volatile uint32_t Data2{ 0 };
	uint8_t CachelinePadding2[64]{};
	volatile uint32_t Data3{ 0 };
	uint8_t CachelinePadding3[64]{};
	volatile uint32_t Data4{ 0 };
	uint8_t CachelinePadding4[64]{};
	volatile uint32_t Data5{ 0 };
	uint8_t CachelinePadding5[64]{};
	volatile uint32_t Data6{ 0 };
	uint8_t CachelinePadding6[64]{};
	volatile uint32_t Data7{ 0 };
};
extern FTestStructure TestStructure0{};
extern FTestStructure TestStructure1{};

void FJobSystem::Init()
{
	SYSTEM_INFO SystemInfo{};
	GetSystemInfo(&SystemInfo);
	const uint32_t LogicalProcessorCount = SystemInfo.dwNumberOfProcessors;
	JobThreadCount = LogicalProcessorCount / 2;

	EA::Thread::ThreadPoolParameters ThreadPoolParam{};
	ThreadPoolParam.mnMaxCount = 0;
	ThreadPoolParam.mnInitialCount = 0;
	ThreadPoolParam.mnIdleTimeoutMilliseconds = EA::Thread::kTimeoutNone; // never destroy job threads
	ThreadPoolParam.mnProcessorMask = GJobThreadAffinityMask;
	ThreadPoolParam.mDefaultThreadParameters.mpName = "JobThread";

	ThreadPool = eastl::make_unique<EA::Thread::ThreadPool>(&ThreadPoolParam);

	for (uint32_t JobIndex = 0; JobIndex < JobThreadCount; ++JobIndex)
	{
		EA::Thread::ThreadParameters ThreadParam{};
		ThreadParam.mnPriority = EA::Thread::kProcessorAny;

		eastl::string ThreadName;
		ThreadName.sprintf("JobThread(%d)", JobIndex);
		ThreadParam.mpName = ThreadName.data();
		//ThreadParam.mnPriority = EA::Thread::kThreadPriorityMin; // no way to increase performance with this yet, only decrease it.

		EA::Thread::MakeThread([ThreadName, ThreadParam, this]() {
			TLSThreadName = ThreadName;
			TLSThreadType = EThreadType::JobThread;
			EA_ASSERT(IsOnJobThread());

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

void FJobSystem::Test()
{
	auto IncrementTestStructure1 = [](FJobDispatchArgs Arg)
	{
		switch (Arg.JobIndex)
		{
			case 0:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data1;
				}
				break;
			}
			case 1:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data2;
				}
				break;
			}
			case 2:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data3;
				}
				break;
			}
			case 3:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data4;
				}
				break;
			}
			case 4:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data5;
				}
				break;
			}
			case 5:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data6;
				}
				break;
			}
			case 6:
			{
				for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
				{
					++TestStructure0.Data7;
				}
				break;
			}
		}
	};

	auto IncrementTestStructure2 = []()
	{
		for (uint32_t JobIndex = 0; JobIndex < 5; ++JobIndex)
		{
			switch (JobIndex)
			{
				case 0:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data1;
					}
					break;
				}
				case 1:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data2;
					}
					break;
				}
				case 2:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data3;
					}
					break;
				}
				case 3:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data4;
					}
					break;
				}
				case 4:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data5;
					}
					break;
				}
				case 5:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data6;
					}
					break;
				}
				case 6:
				{
					for (uint32_t loogindex = 0; loogindex < 100000; ++loogindex)
					{
						++TestStructure0.Data7;
					}
					break;
				}
			}
		}
	};

	{
		FCPUTimer FCPUTimerFJobSystem_Test_UseJobThread176{ "FJobSystem_Test_UseJobThread" };
		Dispatch(7, IncrementTestStructure1, true);

		FCPUTimerFJobSystem_Test_UseJobThread176.End();
		
		RD_LOG(ELogVerbosity::Log, EA_WCHAR("FJobSystem_Test_UseJobThread : %f"), FCPUTimerFJobSystem_Test_UseJobThread176.GetElapsedSeconds());
	}

	{
		FCPUTimer FCPUTimerFJobSystem_Test_WorkOnCallerThread357{ "FJobSystem_Test_WorkOnCallerThread" };
		IncrementTestStructure2();

		FCPUTimerFJobSystem_Test_WorkOnCallerThread357.End();
		RD_LOG(ELogVerbosity::Log, EA_WCHAR("FJobSystem_Test_WorkOnCallerThread : %f"), FCPUTimerFJobSystem_Test_WorkOnCallerThread357.GetElapsedSeconds());
	}
}

void FJobSystem::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
	//Test();
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

eastl::vector<FJobResult> FJobSystem::Dispatch(const uint32_t InJobCount, const JobType& job, const bool bCallerThreadWorkOnJob)
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

	if (bCallerThreadWorkOnJob)
	{
		ProcessJobsOnCallerThread();
	}

	return JobResults;
}

bool FJobSystem::WaitForJobCompletion(const int32_t InJobID, const uint64_t InWaitTimeMilliSeconds /*= EA::Thread::kTimeoutNone*/)
{
	const bool bIsFinished = (ThreadPool->WaitForJobCompletion(InJobID, EA::Thread::ThreadPool::kJobWaitNone /*meaningless param*/, InWaitTimeMilliSeconds) == EA::Thread::ThreadPool::kResultOK);
	return bIsFinished;
}

void FJobSystem::WaitForAllJobCompletion(const uint64_t InWaitTimeMilliSeconds)
{
	ThreadPool->WaitForJobCompletion(-1, EA::Thread::ThreadPool::kJobWaitAll, InWaitTimeMilliSeconds);
}

FJobResult::FJobResult(FJobSystem* const InJobSystem, const int32_t InJobIndex)
	: JobSystem(InJobSystem), JobIndex(InJobIndex)
{

}

bool FJobResult::WaitForJobCompletion(const uint64_t InWaitTimeMilliSeconds)
{
	return JobSystem->WaitForJobCompletion(JobIndex, InWaitTimeMilliSeconds);
}
