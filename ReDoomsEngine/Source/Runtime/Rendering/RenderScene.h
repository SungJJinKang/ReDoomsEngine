#pragma once

#include "CommonInclude.h"
#include "D3D12PSO.h"
#include "EASTL/segmented_vector.h"

#define SEGMENTED_BLOCK_SIZE_FOR_MESH_DRAW 64

struct FMesh;
struct FD3D12CommandContext;

enum class EPass : uint32_t
{
	BasePass,

	Num
};

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
	FD3D12PSOInitializer PSO;
	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	FMeshDrawArgument MeshDrawArgument;

	// This function should be called from render thread
	void Draw(FD3D12CommandContext& InCommandContext);
};

struct FRenderScene
{
	struct FRenderObjectList
	{
		eastl::bitvector<> VisibleFlagsList;
		eastl::vector<BoundingBox> BoundingBoxList;
		/// <summary>
		/// x, y, z : World position
		/// w : Radius of bounding sphere
		/// </summary>
		eastl::vector<Vector4> PositionBoundingSphereRadiusList;
		eastl::vector<Matrix> ModelMatrixList;
		eastl::vector<eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>> VertexBufferViewList;
		eastl::vector<D3D12_INDEX_BUFFER_VIEW> IndexBufferViewList;

		/// <summary>
		/// this can be overridden in CreateMeshDrawListForPass
		/// </summary>
		eastl::vector<FD3D12PSOInitializer::FDrawDesc> DrawDescList;
		eastl::vector<FMeshDrawArgument> MeshDrawArgumentList;

	} RenderObjectList;

	struct FRenderObject
	{
		uint32_t Index; // This value represents index in RenderObjectList
	};

	struct FPass
	{
		FD3D12PSOInitializer::FPassDesc PassPSODesc;

		eastl::segmented_vector<uint32_t, SEGMENTED_BLOCK_SIZE_FOR_MESH_DRAW> DrawedObjectIndexList; // Each element value represents index of render object data list in RenderObjectList
		eastl::segmented_vector<FBoundShaderSet, SEGMENTED_BLOCK_SIZE_FOR_MESH_DRAW> BoundShaderSetList;
	};

	eastl::array<FPass, static_cast<uint32_t>(EPass::Num)> PerPassData;

	uint32_t AddRenderObject(
		const bool bInVisible,
		const BoundingBox& InBoundingBox, 
		const Vector4& InPositionBoundingSphereRadius, 
		const Matrix& InModelMatrix,
		const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
		const D3D12_INDEX_BUFFER_VIEW& IndexBufferView,
		const FD3D12PSOInitializer::FDrawDesc& InDrawDesc,
		const FMeshDrawArgument& InMeshDrawArgument
	);

	uint32_t AddDraw(
		const EPass InPass,
		const uint32_t InObjectIndex,
		const FBoundShaderSet& InBoundShaderSet // need to set proper shader instance for current frame, draw
	);

	// This function will be called from worker thread
	eastl::vector<FMeshDraw> CreateMeshDrawListForPass(const EPass InPass);
};

