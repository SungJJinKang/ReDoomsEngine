#include "Profiling.h"

// reference code : https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12Raytracing/src/D3D12RaytracingProceduralGeometry/util/PerformanceTimers.cpp#L15

#include <windows.h>
#include "EASTL/stack.h"
#include "EASTL/vector.h"
#include "Math/MathUtils.h"

#if ENABLE_PROFILER
#include "D3D12Include.h"
#include "D3D12GlobalVariables.h"
#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"

#include "Editor/ImguiHelper.h"
#include "imgui.h"

#include "EASTL/hash_map.h"
#endif

#define UPDATE_RATE_SHOWN_AVERAGE_TIMER_DATA 0.3f
#define AT_LEAST_UPDATE_RATE_FOR_SHOW_ON_PROFILER_UI 1.0f

static LARGE_INTEGER QPCFrequency{
	[]()->LARGE_INTEGER {
	LARGE_INTEGER QPCFrequency;
	QueryPerformanceFrequency(&QPCFrequency);
	return QPCFrequency;
	}() };

FCPUTimer::FNode::FNode(const char* const InTimerName) 
	: TimerName(InTimerName), ElapsedSeconds(0.0), Average(0.0), ShownAverage(0.0), Childs()
{

}

void FCPUTimer::FNode::RecursiveClearElapsedSeconds()
{
	for (auto ChildNode : Childs)
	{
		ChildNode.second->RecursiveClearElapsedSeconds();
	}
	ElapsedSeconds = 0.0;
}

void FCPUTimer::FNode::RecursiveCalculateAverage(const bool bUpdateShownAverage)
{
	for (auto ChildNode : Childs)
	{
		ChildNode.second->RecursiveCalculateAverage(bUpdateShownAverage);
	}
	
	Average = Lerp(Average, ElapsedSeconds, 0.75);

	if (bUpdateShownAverage)
	{
		ShownAverage = Average;
	}
}

void FCPUTimer::FNode::RecursiveDestory()
{
	for (auto ChildNode : Childs)
	{
		ChildNode.second->RecursiveDestory();
	}
	delete this;
}

// CPU Trace
#if ENABLE_PROFILER
static class FCPUTimerManager
{
public:

	enum class EEventType : uint8_t
	{
		PushStack,
		PopStack
	};

	struct FEvent
	{
		EEventType EventType;
		const char* TimerName;
		float ElapsedSeconds;

		FEvent(const EEventType InEventType, FCPUTimer* const Timer)
			: EventType(InEventType), TimerName(Timer->GetTimerName()), ElapsedSeconds(Timer->GetElapsedSeconds())
		{

		}
	};

	void Destroy()
	{
		for (FCPUTimer::FNode* TopNode : TopNodes)
		{
			TopNode->RecursiveDestory();
		}
		TopNodes.clear();
		PreviousFrameTopNodes.clear();
	}
	void BeginFrame()
	{
		for (FCPUTimer::FNode* TopNode : TopNodes)
		{
			TopNode->RecursiveClearElapsedSeconds();
		}
	}
	void EndFrame()
	{
		ProcessEvents();

		EA_ASSERT(NodeStack.size() == 0);

		ElapsedTimeAfterUpdateShownAverage += GTimeDelta;
		const bool bUpdateShownAverage = ElapsedTimeAfterUpdateShownAverage > UPDATE_RATE_SHOWN_AVERAGE_TIMER_DATA;
		if (bUpdateShownAverage)
		{
			ElapsedTimeAfterUpdateShownAverage = 0.0f;
		}

		for (FCPUTimer::FNode* TopNode : TopNodes)
		{
			TopNode->RecursiveCalculateAverage(bUpdateShownAverage);
		}

		PreviousFrameTopNodes = TopNodes;
	}

	void AddEvent(const FEvent& InEvent)
	{
		Events.push_back(InEvent);
	}

	void ProcessEvents()
	{
		eastl::stack<FCPUTimer::FNode*> OutNodeStack{};
		for (const FEvent& Event : Events)
		{
			switch (Event.EventType)
			{
				case EEventType::PushStack:
				{
					PushStack(Event, OutNodeStack);
					break;
				}
				case EEventType::PopStack:
				{
					PopStack(Event, OutNodeStack);
					break;
				}
				default:
				{
					EA_ASSUME(false);
				}
			}
		}

		EA_ASSERT(OutNodeStack.size() == 0);

		Events.clear();
	}

	const eastl::vector<FCPUTimer::FNode*>& GetPreviousFrameTopNodes() const
	{
		return PreviousFrameTopNodes;
	}
private:

	void PushStack(const FEvent& Event, eastl::stack<FCPUTimer::FNode*>& OutNodeStack)
	{
		FCPUTimer::FNode* Node = nullptr;

		if (NodeStack.size() > 0)
		{
			auto TargetNode = NodeStack.top()->Childs.find(Event.TimerName);
			if (TargetNode && TargetNode != NodeStack.top()->Childs.end())
			{
				Node = TargetNode->second;
			}
			else
			{
				Node = new FCPUTimer::FNode(Event.TimerName);
				NodeStack.top()->Childs.emplace(Event.TimerName, Node);
			}
		}
		else
		{
			FCPUTimer::FNode* const* TargetNode = eastl::find_if(TopNodes.begin(), TopNodes.end(),
				[TimeName = Event.TimerName](FCPUTimer::FNode* lhs) -> bool
				{
					return lhs->TimerName == TimeName; // Just comparing address of literal string is enough
				}
			);
			if (TargetNode && TargetNode != TopNodes.end())
			{
				Node = *TargetNode;
			}
			else
			{
				Node = new FCPUTimer::FNode(Event.TimerName);
				TopNodes.emplace_back(Node);
			}
		}

		OutNodeStack.push(Node);
		NodeStack.push(Node);
	}
	void PopStack(const FEvent& Event, eastl::stack<FCPUTimer::FNode*>& OutNodeStack)
	{
		EA_ASSERT(NodeStack.size() > 0);
		EA_ASSERT(NodeStack.top()->TimerName == Event.TimerName);

		OutNodeStack.top()->ElapsedSeconds += Event.ElapsedSeconds;
		OutNodeStack.pop();

		NodeStack.pop();
	}

	eastl::stack<FCPUTimer::FNode*> NodeStack;
	eastl::vector<FCPUTimer::FNode*> TopNodes;
	eastl::vector<FCPUTimer::FNode*> PreviousFrameTopNodes;
	float ElapsedTimeAfterUpdateShownAverage;

	eastl::vector<FEvent> Events;
} CPUTimerManager;
#endif

FCPUTimer::FCPUTimer(const char* const InTimerName, const bool bInScopedTimer) :
	TimerName(InTimerName),
	QPCLastTime(0),
	ElapsedSeconds(0),
	ElapsedTicks(0),
	bScopedTimer(bInScopedTimer)
{
	if (bScopedTimer)
	{
		Start();
	}
}

FCPUTimer::~FCPUTimer()
{
	if (bScopedTimer)
	{
		End();
	}
}

void FCPUTimer::Start()
{
#if ENABLE_PROFILER
	CPUTimerManager.AddEvent(FCPUTimerManager::FEvent{ FCPUTimerManager::EEventType::PushStack, this});
#endif

	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime); // A Profile shows this function is quiet too slow... (why? https://stackoverflow.com/questions/1723629/what-happens-when-queryperformancecounter-is-called)
	QPCLastTime = CurrentTime.QuadPart;
}

void FCPUTimer::End()
{
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime); // A Profile shows this function is quiet too slow... (why? https://stackoverflow.com/questions/1723629/what-happens-when-queryperformancecounter-is-called)

	UINT64 TimeDelta = CurrentTime.QuadPart - QPCLastTime;

	// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
	TimeDelta *= TicksPerSecond;
	TimeDelta /= QPCFrequency.QuadPart;

	ElapsedTicks = TimeDelta;
	ElapsedSeconds = TicksToSeconds(ElapsedTicks);

#if ENABLE_PROFILER
	CPUTimerManager.AddEvent(FCPUTimerManager::FEvent{ FCPUTimerManager::EEventType::PopStack, this });
#endif
	QPCLastTime = CurrentTime.QuadPart;
}

#if ENABLE_PROFILER
// Memory Trace

extern eastl::vector<const char*> MemoryTraceStack{};

// use literal string address as key
extern eastl::hash_map<const char*, FMemoryTraceData> TraceDataMap{};

FScopedMemoryTrace::FScopedMemoryTrace(const char* const InTraceName)
	: TraceName(InTraceName)
{
	FMemoryTraceData& TraceData = TraceDataMap.try_emplace(InTraceName).first->second;
	TraceData.TraceName = InTraceName;

	if (MemoryTraceStack.size() > 0)
	{
		EA_ASSERT(MemoryTraceStack.back() != InTraceName);
	}
	MemoryTraceStack.emplace_back(InTraceName);
}

FScopedMemoryTrace::~FScopedMemoryTrace()
{
	EA_ASSERT(MemoryTraceStack.back() == TraceName);
	MemoryTraceStack.pop_back();
}

// GPU Trace
static constexpr uint64_t GPU_TRACE_DATA_MAX_COUNT = 50;
static constexpr uint64_t GPU_TRACE_DATA_SLOT_COUNT = GPU_TRACE_DATA_MAX_COUNT * 2;

struct FGPUTimerData
{
	const char* TimerName;
	double Average;
	double ShownAverage;
	float NonUpdatedDuration = 0.0f;
	bool bUpdatedCurrentFrame{ false };
};

static class FGPUTimerManager
{
public:

	bool IsInit() const
	{
		return GPUTimerBuffer;
	}

	void InitIfRequired(FD3D12CommandQueue* const InCommandQueue)
	{
		if (!IsInit())
		{
			Init(InCommandQueue);
		}
	}

	void Destroy()
	{

	}

	// Indicate beginning & end of frame
	void BeginFrame(FD3D12CommandContext* const InD3D12CommandContext)
	{
		GPUTimerManager.InitIfRequired(InD3D12CommandContext->CommandQueueList[ED3D12QueueType::Direct]);

		if (IsInit())
		{
			SCOPED_CPU_TIMER(FGPUTimerManager_BeginFrame)

			uint64_t LocalCurrentAllocatedTimerIDCount = CurrentAllocatedTimerIDCount;
			for (uint32_t AllocatedTimerID = 0; AllocatedTimerID < LocalCurrentAllocatedTimerIDCount; ++AllocatedTimerID)
			{
				FGPUTimerData& GPUTimerData = GPUTimerDataList[AllocatedTimerID];
				GPUTimerData.bUpdatedCurrentFrame = false;
			}

			CurrentD3D12CommandContext = InD3D12CommandContext;
		}
	}
	void EndFrame(FD3D12CommandContext* const InD3D12CommandContext)
	{
		if (IsInit())
		{
			SCOPED_CPU_TIMER(FGPUTimerManager_EndFrame)

			static uint32_t FrameIndex = 0;
			// Resolve query for the current frame.
			uint64_t ResolveToBaseAddress = FrameIndex * GPU_TRACE_DATA_SLOT_COUNT * sizeof(UINT64);
			InD3D12CommandContext->GraphicsCommandList->GetD3DCommandList()->ResolveQueryData(GPUTimerHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, GPU_TRACE_DATA_SLOT_COUNT, GPUTimerBuffer.Get(), ResolveToBaseAddress);

			// Grab read-back data for the queries from a finished frame MaxframeCount ago.                                                           
			uint32_t ReadBackFrameID = (FrameIndex + 1) % (MaxFrameCount + 1);
			size_t ReadBackBaseOffset = ReadBackFrameID * GPU_TRACE_DATA_SLOT_COUNT * sizeof(UINT64);
			D3D12_RANGE DataRange =
			{
				ReadBackBaseOffset,
				ReadBackBaseOffset + GPU_TRACE_DATA_SLOT_COUNT * sizeof(UINT64),
			};

			UINT64* QueriedTimingData;
			VERIFYD3D12RESULT(GPUTimerBuffer->Map(0, &DataRange, reinterpret_cast<void**>(&QueriedTimingData)));
			EA::StdC::Memcpy(TimingData, QueriedTimingData, sizeof(UINT64) * GPU_TRACE_DATA_SLOT_COUNT);
			GPUTimerBuffer->Unmap(0, nullptr);

			ElapsedTimeAfterUpdateShownAverage += GTimeDelta;
			const bool bUpdateShownAverage = ElapsedTimeAfterUpdateShownAverage > UPDATE_RATE_SHOWN_AVERAGE_TIMER_DATA;
			if (bUpdateShownAverage)
			{
				ElapsedTimeAfterUpdateShownAverage = 0.0f;
			}

			uint64_t LocalCurrentAllocatedTimerIDCount = CurrentAllocatedTimerIDCount;
			for (uint32_t AllocatedTimerID = 0; AllocatedTimerID < LocalCurrentAllocatedTimerIDCount; ++AllocatedTimerID)
			{
				FGPUTimerData& GPUTimerData = GPUTimerDataList[AllocatedTimerID];

				double Delta = 0.0f;
				if (GPUTimerData.bUpdatedCurrentFrame)
				{
					uint64_t Start = TimingData[AllocatedTimerID * 2];
					uint64_t End = TimingData[AllocatedTimerID * 2 + 1];
					Delta = double(End - Start)* GPUFreqInv;
				}

				GPUTimerData.Average = Lerp(GPUTimerData.Average, Delta, 0.75);
				
				if (bUpdateShownAverage)
				{
					GPUTimerData.ShownAverage = GPUTimerData.Average;
				}
				if (!(GPUTimerData.bUpdatedCurrentFrame))
				{
					GPUTimerData.NonUpdatedDuration += GTimeDelta;
				}
				else
				{
					GPUTimerData.NonUpdatedDuration = 0.0f;
				}
			}

			FrameIndex = ReadBackFrameID;

			CurrentD3D12CommandContext = nullptr;
		}
	}

	// Start/stop a particular performance timer (don't start same index more than once in a single frame)
	void Start(FD3D12CommandList* const InCommandList, const char* const InTimerName)
	{
		if (IsInit())
		{
			EA_ASSERT(CurrentD3D12CommandContext);

			uint32_t TimerID = GetOrCreateTimerID(InTimerName);

			InCommandList->GetD3DCommandList()->EndQuery(GPUTimerHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, TimerID * 2);
		}
	}
	void Stop(FD3D12CommandList* const InCommandList, const char* const InTimerName)
	{
		if (IsInit())
		{
			EA_ASSERT(CurrentD3D12CommandContext);

			uint32_t TimerID = GetOrCreateTimerID(InTimerName);

			FGPUTimerData& GPUTimerData = GPUTimerDataList[TimerID];
			GPUTimerData.bUpdatedCurrentFrame = true;

			InCommandList->GetD3DCommandList()->EndQuery(GPUTimerHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, TimerID * 2 + 1);
		}
	}

	const eastl::array<FGPUTimerData, GPU_TRACE_DATA_MAX_COUNT>& GetGPUTimerDataList(uint64_t& OutCurrentAllocatedTimerIDCount)
	{
		OutCurrentAllocatedTimerIDCount = CurrentAllocatedTimerIDCount;
		return GPUTimerDataList;
	}


private:

	void Init(FD3D12CommandQueue* const InCommandQueue)
	{
		ID3D12Device* const Device = GetD3D12Device();

		// Filter a debug warning coming when accessing a readback resource for the timing queries.
		// The readback resource handles multiple frames data via per-frame offsets within the same resource and CPU
		// maps an offset written "frame_count" frames ago and the data is guaranteed to had been written to by GPU by this time. 
		// Therefore the race condition doesn't apply in this case.
		ComPtr<ID3D12InfoQueue> D3dInfoQueue;
		if (SUCCEEDED(Device->QueryInterface(IID_PPV_ARGS(&D3dInfoQueue))))
		{
			// Suppress individual messages by their ID.
			D3D12_MESSAGE_ID DenyIds[] =
			{
				D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_GPU_WRITTEN_READBACK_RESOURCE_MAPPED,
			};

			D3D12_INFO_QUEUE_FILTER Filter = {};
			Filter.DenyList.NumIDs = _countof(DenyIds);
			Filter.DenyList.pIDList = DenyIds;
			D3dInfoQueue->AddStorageFilterEntries(&Filter);
			//OutputDebugString(L"Warning: GPUTimer is disabling an unwanted D3D12 debug layer warning: D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_GPU_WRITTEN_READBACK_RESOURCE_MAPPED.");
		}

		uint64_t GPUFreq;
		VERIFYD3D12RESULT(InCommandQueue->GetD3DCommandQueue()->GetTimestampFrequency(&GPUFreq));
		GPUFreqInv = 1000.0 / double(GPUFreq);

		D3D12_QUERY_HEAP_DESC Desc = {};
		Desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		Desc.Count = GPU_TRACE_DATA_SLOT_COUNT;
		VERIFYD3D12RESULT(Device->CreateQueryHeap(&Desc, IID_PPV_ARGS(GPUTimerHeap.ReleaseAndGetAddressOf())));
		GPUTimerHeap->SetName(L"GPUTimerHeap");

		CD3DX12_HEAP_PROPERTIES ReadBackHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK); // reside on system memory

		// We allocate MaxframeCount + 1 instances as an instance is guaranteed to be written to if maxPresentFrameCount frames
		// have been submitted since. This is due to a fact that Present stalls when none of the MaxframeCount frames are done/available.
		size_t PerFrameInstances = GNumBackBufferCount + 1;

		CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(PerFrameInstances * GPU_TRACE_DATA_SLOT_COUNT * sizeof(UINT64));
		VERIFYD3D12RESULT(Device->CreateCommittedResource(
			&ReadBackHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&BufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(GPUTimerBuffer.ReleaseAndGetAddressOf()))
		);
		GPUTimerBuffer->SetName(L"GPUTimerBuffer");
	}

	uint32_t GetOrCreateTimerID(const char* const InTimerName)
	{
		uint32_t TimerID = 0;

		auto AllocatedTimerID = AllocatedTimerIDMap.find(InTimerName);
		if (AllocatedTimerID == AllocatedTimerIDMap.end())
		{
			TimerID = CurrentAllocatedTimerIDCount++;
			EA_ASSERT(TimerID <= GPU_TRACE_DATA_MAX_COUNT);

			AllocatedTimerIDMap.emplace(InTimerName, TimerID);
			GPUTimerDataList[TimerID].TimerName = InTimerName;
		}
		else
		{
			TimerID = AllocatedTimerID->second;
		}

		return TimerID;	
	}

	eastl::atomic<uint32_t> CurrentAllocatedTimerIDCount{ 0 };
	eastl::array<FGPUTimerData, GPU_TRACE_DATA_MAX_COUNT> GPUTimerDataList{};
	eastl::hash_map<const char*, uint32_t> AllocatedTimerIDMap{};

	Microsoft::WRL::ComPtr<ID3D12QueryHeap> GPUTimerHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource>  GPUTimerBuffer;
	double                                  GPUFreqInv;
	uint64_t                                TimingData[GPU_TRACE_DATA_SLOT_COUNT];
	size_t                                  MaxFrameCount;
	FD3D12CommandContext* CurrentD3D12CommandContext{ nullptr };

	float ElapsedTimeAfterUpdateShownAverage;
} GPUTimerManager{};

FGPUTimer::FGPUTimer(const char* const InTimerName)
	: TargetCommandList(), TimerName(InTimerName), bScoped(false)
{

}

FGPUTimer::FGPUTimer(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList, const char* const InTimerName)
	: TargetCommandList(InCommandList), TimerName(InTimerName), bScoped(true)
{
	if (bScoped)
	{
		Start(InCommandQueue, TargetCommandList);
	}
}

FGPUTimer::~FGPUTimer()
{
	if (bScoped)
	{
		End(TargetCommandList);
	}
}

void FGPUTimer::Start(FD3D12CommandQueue* const InCommandQueue, FD3D12CommandList* const InCommandList)
{
	GPUTimerManager.Start(InCommandList, TimerName);
}

void FGPUTimer::End(FD3D12CommandList* const InCommandList)
{
	GPUTimerManager.Stop(InCommandList, TimerName);
}


static struct FRegisterProfilerImguiCallback
{
	static void RecursiveDrawCPUTimer(const FCPUTimer::FNode* const InNode)
	{
		ImGui::Indent();

		ImGui::Text("%s : %f(ms)", InNode->TimerName, InNode->ShownAverage * 1000.0);
		for (auto& Child : InNode->Childs)
		{
			RecursiveDrawCPUTimer(Child.second);
		}

		ImGui::Unindent();
	}

	FRegisterProfilerImguiCallback()
	{
		FImguiHelperSingleton::GetInstance()->ImguiDrawEventList.emplace_back([]() {
			if (!ImGui::Begin("ReDoomsEngine", NULL, 0))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}
			else
			{
				// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
				// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
				//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
				// e.g. Leave a fixed amount of width for labels (by passing a negative Delta), the rest goes to widgets.
				ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

				ImGui::Text("DrawCall : %u", GPreviousFrameDrawCallCount);

				const eastl::vector<FCPUTimer::FNode*>& TopNodes = CPUTimerManager.GetPreviousFrameTopNodes();

				const FCPUTimer::FNode* const* FrameTimeCPUTimerNode = eastl::find_if(TopNodes.begin(), TopNodes.end(),
					[](FCPUTimer::FNode* lhs) -> bool
					{
						return lhs->TimerName == "FrameTime"; // Just comparing address of literal string is enough
					}
				);

				if(FrameTimeCPUTimerNode && FrameTimeCPUTimerNode != TopNodes.end())
				{
					ImGui::Text("FPS : %f", 1.0 / (*FrameTimeCPUTimerNode)->ShownAverage);
					ImGui::Text("FrameTime : %f(ms)", (*FrameTimeCPUTimerNode)->ShownAverage * 1000.0);
				}

				if (ImGui::TreeNode("CPU Timer"))
				{
					for (FCPUTimer::FNode* TopNode : TopNodes)
					{
						RecursiveDrawCPUTimer(TopNode);
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("GPU Timer"))
				{
					uint64_t OutCurrentAllocatedTimerIDCount;
					const eastl::array<FGPUTimerData, GPU_TRACE_DATA_MAX_COUNT>& GPUTimerDataList = GPUTimerManager.GetGPUTimerDataList(OutCurrentAllocatedTimerIDCount);

					for (uint32_t AllocatedTimerID = 0; AllocatedTimerID < OutCurrentAllocatedTimerIDCount; ++AllocatedTimerID)
					{
						if (GPUTimerDataList[AllocatedTimerID].NonUpdatedDuration < AT_LEAST_UPDATE_RATE_FOR_SHOW_ON_PROFILER_UI)
						{
							ImGui::Text("%s : %f(ms)", GPUTimerDataList[AllocatedTimerID].TimerName, GPUTimerDataList[AllocatedTimerID].ShownAverage);
						}
					}

					ImGui::TreePop();
				}

				// End of ShowDemoWindow()
				ImGui::PopItemWidth();
				ImGui::End();
			}
			});
	}
} RegisterProfilerImguiCallback{};

void CPUTimerBeginFrame()
{
	CPUTimerManager.BeginFrame();
}
void CPUTimerEndFrame()
{
	CPUTimerManager.EndFrame();
}
void GPUTimerBeginFrame(FD3D12CommandContext* const InD3D12CommandContext)
{
	GPUTimerManager.BeginFrame(InD3D12CommandContext);
}
void GPUTimerEndFrame(FD3D12CommandContext* const InD3D12CommandContext)
{
	GPUTimerManager.EndFrame(InD3D12CommandContext);
}

void DestroyTimerData()
{

}
#endif
