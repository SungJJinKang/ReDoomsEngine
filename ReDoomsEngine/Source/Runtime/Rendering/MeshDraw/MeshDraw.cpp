#include "MeshDraw.h"

#include "D3D12CommandContext.h"
#include "D3D12PSO.h"

void FMeshDraw::Draw(FD3D12CommandContext& InCommandContext)
{
	InCommandContext.StateCache.SetVertexBufferViewList(VertexBufferViewList);
	InCommandContext.StateCache.SetIndexBufferView(IndexBufferView);

	InCommandContext.StateCache.SetPSO(PSO);

	if (MeshDrawArgument.IndexCountPerInstance > 0)
	{
		InCommandContext.DrawIndexedInstanced(
			MeshDrawArgument.IndexCountPerInstance,
			MeshDrawArgument.InstanceCount,
			MeshDrawArgument.StartIndexLocation,
			MeshDrawArgument.BaseVertexLocation,
			MeshDrawArgument.StartInstanceLocation);
	}
	else
	{
		InCommandContext.DrawInstanced(
			MeshDrawArgument.VertexCountPerInstance,
			MeshDrawArgument.InstanceCount,
			MeshDrawArgument.StartVertexLocation,
			MeshDrawArgument.StartInstanceLocation);
	}
}
