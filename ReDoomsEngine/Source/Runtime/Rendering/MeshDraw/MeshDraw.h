#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"
#include "D3D12PSO.h"

struct FD3D12CommandContext;
struct FMeshDrawArgument
{
	uint32_t VertexCountPerInstance;
	uint32_t StartVertexLocation;

	// if this value is not zero, use DrawIndexedInstanced
	uint32_t IndexCountPerInstance;
	uint32_t StartIndexLocation;
	uint32_t InstanceCount;

	int32_t BaseVertexLocation;
	uint32_t StartInstanceLocation;
};

/// <summary>
/// This struct represent a draw call.
/// This struct should contains all datas for a draw call
/// </summary>
struct FMeshDraw
{
	bool bIsValid = false;
	FD3D12PSOInitializer PSO;
	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	FMeshDrawArgument MeshDrawArgument;
	eastl::fixed_vector<uint32, 1> PrimitiveIdList;

	uint64 CachedHash;

	void CacheHash();

	void PrepareDraw(FD3D12CommandContext& InCommandContext, FD3D12ConstantBufferResource& InPrimitiveIDBuffer);
	// This function should be called from render thread
	void Draw(FD3D12CommandContext& InCommandContext, FD3D12ConstantBufferResource& InPrimitiveIDBuffer);

	bool operator==(const FMeshDraw& rhs) const;
};

bool CanMergeMeshDraw(const FMeshDraw& InMeshDrawA, const FMeshDraw& InMeshDrawB);

