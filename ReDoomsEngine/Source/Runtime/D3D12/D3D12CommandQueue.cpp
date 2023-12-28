#include "D3D12CommandQueue.h"
#include "D3D12Device.h"

D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType(ED3D12QueueType QueueType)
{
	switch (QueueType)
	{
	case ED3D12QueueType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case ED3D12QueueType::Copy:   return D3D12_COMMAND_LIST_TYPE_COPY;
	case ED3D12QueueType::Async:  return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	default: EA_ASSUME(0); // fallthrough
	}
}

const wchar_t* GetD3D12QueueTypeString(ED3D12QueueType QueueType)
{
	switch (QueueType)
	{
	case ED3D12QueueType::Direct: return EA_WCHAR("Direct");
	case ED3D12QueueType::Copy:   return EA_WCHAR("Copy");
	case ED3D12QueueType::Async:  return EA_WCHAR("Async");
	default: EA_ASSUME(0); // fallthrough
	}
}


FD3D12CommandQueue::FD3D12CommandQueue(FD3D12Device* const InDevice, const ED3D12QueueType InQueueType)
	: Device(InDevice), QueueType(InQueueType), CommandQueue(), Fence()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
	CommandQueueDesc.Type = GetD3D12CommandListType((ED3D12QueueType)QueueType);
	CommandQueueDesc.Priority = 0;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	VERIFYD3D12RESULT(Device->GetD3D12Device()->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue)));
	CommandQueue->SetName(eastl::wstring{ eastl::wstring::CtorSprintf(), EA_WCHAR("%s Queue"), GetD3D12QueueTypeString(QueueType)}.c_str());

	VERIFYD3D12RESULT(Device->GetD3D12Device()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&(Fence.D3DFence))
	));
	Fence.D3DFence->SetName(eastl::wstring{ eastl::wstring::CtorSprintf(), EA_WCHAR("%s Queue Fence"), GetD3D12QueueTypeString(QueueType) }.c_str());
}

