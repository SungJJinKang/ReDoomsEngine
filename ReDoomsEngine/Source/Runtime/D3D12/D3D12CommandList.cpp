#include "D3D12CommandList.h"
#include "D3D12Device.h"

D3D12CommandList::D3D12CommandList(const ED3D12QueueType InQueueType)
	: QueueType(InQueueType), CommandAllocator(), CommandList()
{
}

void D3D12CommandList::Init()
{
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandAllocator(GetD3D12CommandListType((ED3D12QueueType)QueueType), IID_PPV_ARGS(&CommandAllocator)));
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandList(0, GetD3D12CommandListType(QueueType), CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)));
}
