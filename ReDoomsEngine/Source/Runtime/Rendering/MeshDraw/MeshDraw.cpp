#include "MeshDraw.h"

#include "D3D12CommandContext.h"
#include "D3D12PSO.h"

void FMeshDraw::PrepareDraw()
{
}

void FMeshDraw::Draw(FD3D12CommandContext& InCommandContext, const FD3D12VertexIndexBufferResource& InPrimitiveIDBuffer, const uint32 InPrimitiveIndex)
{
	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> UsedVertexBufferViewList
		= VertexBufferViewList;

	EA_ASSERT(GSupportedMaxPrimitiveCount > InPrimitiveIndex);
	D3D12_VERTEX_BUFFER_VIEW PrimitiveIDBufferView{};
	PrimitiveIDBufferView.BufferLocation = InPrimitiveIDBuffer.GPUVirtualAddress() + InPrimitiveIndex * sizeof(uint32);
	PrimitiveIDBufferView.SizeInBytes = InPrimitiveIDBuffer.GetBufferSize() - InPrimitiveIndex * sizeof(uint32);
	PrimitiveIDBufferView.StrideInBytes = InPrimitiveIDBuffer.GetDefaultStrideInBytes();
	UsedVertexBufferViewList.emplace_back(PrimitiveIDBufferView);
	EA_ASSERT(UsedVertexBufferViewList.size() == (PRIMITIVE_ID_INPUT_ELEMENT_SLOT_INDEX + 1));

	InCommandContext.StateCache.SetVertexBufferViewList(UsedVertexBufferViewList);
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
