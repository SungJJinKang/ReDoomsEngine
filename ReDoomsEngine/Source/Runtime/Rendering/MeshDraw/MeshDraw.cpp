#include "MeshDraw.h"

#include "D3D12CommandContext.h"
#include "D3D12PSO.h"

void FMeshDraw::PrepareDraw(FD3D12CommandContext& InCommandContext, FD3D12ConstantBufferResource& InPrimitiveIDBuffer)
{
	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> UsedVertexBufferViewList
		= VertexBufferViewList;

	D3D12_VERTEX_BUFFER_VIEW PrimitiveIDBufferView{};

	InPrimitiveIDBuffer.Versioning(sizeof(uint32) * PrimitiveIdList.size());
	uint32* const PrimitiveIDBuffer = reinterpret_cast<uint32*>(InPrimitiveIDBuffer.GetMappedAddress());
	for (uint32 PrimitiveIDIndex = 0; PrimitiveIDIndex < PrimitiveIdList.size(); ++PrimitiveIDIndex)
	{
		EA_ASSERT(GSupportedMaxPrimitiveCount > PrimitiveIdList[PrimitiveIDIndex]);
		*(PrimitiveIDBuffer + PrimitiveIDIndex) = PrimitiveIdList[PrimitiveIDIndex];
	}
	PrimitiveIDBufferView.BufferLocation = InPrimitiveIDBuffer.GPUVirtualAddress();
	PrimitiveIDBufferView.SizeInBytes = sizeof(uint32) * PrimitiveIdList.size();
	PrimitiveIDBufferView.StrideInBytes = sizeof(uint32);
	UsedVertexBufferViewList.emplace_back(PrimitiveIDBufferView);
	EA_ASSERT(UsedVertexBufferViewList.size() == (PRIMITIVE_ID_INPUT_ELEMENT_SLOT_INDEX + 1));

	InCommandContext.StateCache.SetVertexBufferViewList(UsedVertexBufferViewList);
	InCommandContext.StateCache.SetIndexBufferView(IndexBufferView);

	InCommandContext.StateCache.SetPSO(PSO);

}

void FMeshDraw::Draw(FD3D12CommandContext& InCommandContext, FD3D12ConstantBufferResource& InPrimitiveIDBuffer)
{
	PrepareDraw(InCommandContext, InPrimitiveIDBuffer);
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
