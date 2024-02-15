#include "D3D12CommandQueue.h"
#include "D3D12Device.h"
#include "D3D12CommandList.h"

D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType(ED3D12QueueType QueueType)
{
	switch (QueueType)
	{
	case ED3D12QueueType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case ED3D12QueueType::Copy:   return D3D12_COMMAND_LIST_TYPE_COPY;
	case ED3D12QueueType::Async:  return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	default: EA_ASSUME(0);
	}
}

const wchar_t* GetD3D12QueueTypeString(ED3D12QueueType QueueType)
{
	switch (QueueType)
	{
	case ED3D12QueueType::Direct: return EA_WCHAR("Direct");
	case ED3D12QueueType::Copy:   return EA_WCHAR("Copy");
	case ED3D12QueueType::Async:  return EA_WCHAR("Async");
	default: EA_ASSUME(0);
	}
}


FD3D12CommandQueue::FD3D12CommandQueue(const ED3D12QueueType InQueueType)
	: QueueType(InQueueType), D3DCommandQueue(), Fence()
{
}

void FD3D12CommandQueue::Init()
{
	Fence.InitIfRequired();

	EA_ASSERT(D3DCommandQueue.Get() == nullptr);

	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	MEM_ZERO(CommandQueueDesc);
	CommandQueueDesc.Type = GetD3D12CommandListType((ED3D12QueueType)QueueType);
	CommandQueueDesc.Priority = 0;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&D3DCommandQueue)));
	D3DCommandQueue->SetName(eastl::wstring{ eastl::wstring::CtorSprintf(), EA_WCHAR("%s Queue"), GetD3D12QueueTypeString(QueueType) }.c_str());

	Fence.SetDebugNameToFence(eastl::wstring{ eastl::wstring::CtorSprintf(), EA_WCHAR("%s Queue Fence"), GetD3D12QueueTypeString(QueueType) }.c_str());
}

void FD3D12CommandQueue::WaitForCompletion()
{
	Fence.Signal(this, true);
}

void FD3D12CommandQueue::ExecuteCommandLists(eastl::vector<eastl::shared_ptr<FD3D12CommandList>>& CommandLists)
{
	eastl::vector<ID3D12CommandList*> D3D12CommandLists{};

	for (eastl::shared_ptr<FD3D12CommandList>& CommandList : CommandLists)
	{
		CommandList->FinishRecordingCommandList();
		D3D12CommandLists.emplace_back(CommandList->GetD3DCommandList());
	}

	GetD3DCommandQueue()->ExecuteCommandLists(D3D12CommandLists.size(), D3D12CommandLists.data());
}
