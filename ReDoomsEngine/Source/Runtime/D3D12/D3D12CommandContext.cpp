#include "D3D12CommandContext.h"

#include "D3D12CommandList.h"

void FD3D12CommandContext::DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation, uint32_t StartInstanceLocation)
{
	StateCache.Flush(*this, EPipeline::Graphics);
	++GDrawCallCount;
	{
		SCOPED_CPU_TIMER(D3DCommandList_DrawInstanced)
		GraphicsCommandList->GetD3DCommandList()->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}
}

void FD3D12CommandContext::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StartIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation)
{
	StateCache.Flush(*this, EPipeline::Graphics);
	++GDrawCallCount;

	{
		SCOPED_CPU_TIMER(D3DCommandList_DrawIndexedInstanced)
		GraphicsCommandList->GetD3DCommandList()->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}
}

void FD3D12CommandContext::Dispatch(uint32_t ThreadGroupCountX, uint32_t ThreadGroupCountY, uint32_t ThreadGroupCountZ)
{
	StateCache.Flush(*this, EPipeline::Compute);
	GraphicsCommandList->GetD3DCommandList()->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}
