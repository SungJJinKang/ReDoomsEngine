#include "D3D12ResourceBarrierBatcher.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"
#include "Utils/ConsoleVariable.h"

static TConsoleVariable<bool> GImmediatelyFlushResourceBarrierBatcher{ "r.ImmediatelyFlushResourceBarrierBatcher", true };

FD3D12ResourceBarrierBatcher::FD3D12ResourceBarrierBatcher(FD3D12CommandList& InOwnerCommandList)
	: OwnerCommandList(InOwnerCommandList), PendingResourceBarriers()
{

}

void FD3D12ResourceBarrierBatcher::Flush()
{
	if (!PendingResourceBarriers.empty())
	{
		OwnerCommandList.GetD3DCommandList()->ResourceBarrier(PendingResourceBarriers.size(), PendingResourceBarriers.data());
		PendingResourceBarriers.clear();
	}
}

void FD3D12ResourceBarrierBatcher::AddBarrier(const CD3DX12_RESOURCE_BARRIER& InBarrier)
{
	const D3D12_RESOURCE_BARRIER& Barrier = InBarrier;
	EA_ASSERT(
		(InBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) ? (Barrier.Transition.StateBefore != Barrier.Transition.StateAfter) : true);
	PendingResourceBarriers.emplace_back(InBarrier);

	if (GImmediatelyFlushResourceBarrierBatcher)
	{
		Flush();
	}
}