#include "D3D12CommandContext.h"

#include "D3D12CommandList.h"

void FD3D12CommandContext::DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation, uint32_t StartInstanceLocation)
{
	GraphicsCommandList->ResourceBarrierBatcher.Flush(*GraphicsCommandList);
	StateCache.Flush(*this, EPipeline::Graphics);
	++GDrawCallCount;
	{
		SCOPED_CPU_TIMER(D3DCommandList_DrawInstanced)
		GraphicsCommandList->GetD3DCommandList()->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}
}

void FD3D12CommandContext::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StartIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation)
{
	GraphicsCommandList->ResourceBarrierBatcher.Flush(*GraphicsCommandList);
	StateCache.Flush(*this, EPipeline::Graphics);
	++GDrawCallCount;

	{
		SCOPED_CPU_TIMER(D3DCommandList_DrawIndexedInstanced)
		GraphicsCommandList->GetD3DCommandList()->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}
}

void FD3D12CommandContext::Dispatch(uint32_t ThreadGroupCountX, uint32_t ThreadGroupCountY, uint32_t ThreadGroupCountZ)
{
	GraphicsCommandList->ResourceBarrierBatcher.Flush(*GraphicsCommandList);
	StateCache.Flush(*this, EPipeline::Compute);
	GraphicsCommandList->GetD3DCommandList()->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void FD3D12CommandContext::FlushCommandList(const ED3D12QueueType InD3D12QueueType, const bool InWaitOnCompletation)
{
	FD3D12CommandQueue* const TargetCommandQueue = CommandQueueList[InD3D12QueueType];

	if (InD3D12QueueType == ED3D12QueueType::Direct)
	{
		eastl::vector<eastl::shared_ptr<FD3D12CommandList>> CommandLists = { GraphicsCommandList };
		TargetCommandQueue->ExecuteCommandLists(CommandLists);
	}
	else
	{
		EA_ASSERT(false);
	}

	if (InWaitOnCompletation)
	{
		TargetCommandQueue->WaitForCompletion();
	}
}
