#include "D3D12ResourceBarrierBatcher.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"

void FD3D12ResourceBarrierBatcher::Flush(FD3D12CommandList& InCommandList)
{
	if (!PendingResourceBarriers.empty())
	{
		InCommandList.GetD3DCommandList()->ResourceBarrier(PendingResourceBarriers.size(), PendingResourceBarriers.data());
		PendingResourceBarriers.clear();
	}
}

void FD3D12ResourceBarrierBatcher::AddBarrier(const CD3DX12_RESOURCE_BARRIER& InBarrier)
{
	PendingResourceBarriers.emplace_back(InBarrier);
}

void FD3D12ResourceBarrierBatcher::AddBarrier(const eastl::vector<CD3DX12_RESOURCE_BARRIER>& InBarriers)
{
	PendingResourceBarriers.insert(PendingResourceBarriers.end(), InBarriers.begin(), InBarriers.end());
}
