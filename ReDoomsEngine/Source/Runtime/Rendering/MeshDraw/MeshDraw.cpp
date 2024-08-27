#include "MeshDraw.h"

#include "D3D12CommandContext.h"
#include "D3D12PSO.h"

void FMeshDraw::CacheHash()
{
	CachedHash = 0;
	CachedHash ^= PSO.GetCachedHash();

	for (D3D12_VERTEX_BUFFER_VIEW& VertexBufferView : VertexBufferViewList)
	{
		CachedHash ^= eastl::hash<D3D12_VERTEX_BUFFER_VIEW>()(VertexBufferView);
	}

	CachedHash ^= eastl::hash<D3D12_INDEX_BUFFER_VIEW>()(IndexBufferView);
}

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

bool FMeshDraw::operator==(const FMeshDraw& rhs) const
{
	return CanMergeMeshDraw(*this, rhs);
}

bool CanMergeMeshDraw(const FMeshDraw& InMeshDrawA, const FMeshDraw& InMeshDrawB)
{
	bool bCanMerge = true;

	if (InMeshDrawA.CachedHash != InMeshDrawB.CachedHash)
	{
		bCanMerge = false;
	}

	if (bCanMerge)
	{
		if (InMeshDrawA.PSO.GetCachedHash() != InMeshDrawB.PSO.GetCachedHash())
		{
			bCanMerge = false;
		}
	}

	if (bCanMerge)
	{
		if (InMeshDrawA.VertexBufferViewList.size() != InMeshDrawB.VertexBufferViewList.size())
		{
			bCanMerge = false;
		}
		else
		{
			for (uint32_t VertexBufferViewIndex = 0; VertexBufferViewIndex < InMeshDrawA.VertexBufferViewList.size(); ++VertexBufferViewIndex)
			{
				if (InMeshDrawA.VertexBufferViewList[VertexBufferViewIndex] != InMeshDrawB.VertexBufferViewList[VertexBufferViewIndex])
				{
					bCanMerge = false;
					break;
				}
			}
		}
	}

	if (bCanMerge)
	{
		if (InMeshDrawA.IndexBufferView != InMeshDrawB.IndexBufferView)
		{
			bCanMerge = false;
		}
	}

	if (bCanMerge)
	{
		if (
			(InMeshDrawA.MeshDrawArgument.VertexCountPerInstance != InMeshDrawB.MeshDrawArgument.VertexCountPerInstance) ||
			(InMeshDrawA.MeshDrawArgument.StartVertexLocation != InMeshDrawB.MeshDrawArgument.StartVertexLocation) ||
			(InMeshDrawA.MeshDrawArgument.IndexCountPerInstance != InMeshDrawB.MeshDrawArgument.IndexCountPerInstance) ||
			(InMeshDrawA.MeshDrawArgument.StartIndexLocation != InMeshDrawB.MeshDrawArgument.StartIndexLocation) ||
			(InMeshDrawA.MeshDrawArgument.BaseVertexLocation != InMeshDrawB.MeshDrawArgument.BaseVertexLocation) ||
			(InMeshDrawA.MeshDrawArgument.StartInstanceLocation != InMeshDrawB.MeshDrawArgument.StartInstanceLocation)
			)
		{
			bCanMerge = false;
		}
	}

	return bCanMerge;
}