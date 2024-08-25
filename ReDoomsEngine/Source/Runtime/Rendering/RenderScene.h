#pragma once

#include "CommonInclude.h"
#include "D3D12PSO.h"
#include "EASTL/segmented_vector.h"
#include "MeshDraw/MeshDraw.h"
#include "D3D12Shader.h"
#include "SceneData/GPUScene.h"

struct FMesh;
struct FD3D12CommandContext;

enum class EPass : uint32_t
{
	BasePass,

	Num
};

enum EPrimitiveFlag : uint32
{
	CacheMeshDrawCommand = 1 << 0,
	AllowMergeMeshDrawCommand = 1 << 1,
};

struct FPrimitiveList
{
	eastl::array<eastl::bitvector<>, static_cast<uint32_t>(EPass::Num)> VisibleFlagsList;
	eastl::bitvector<> TransformDirtyPrimitiveList;
	eastl::bitvector<> GPUSceneDirtyPrimitiveList;
	eastl::vector<BoundingBox> WorldSpaceBoundingBoxList;
	eastl::vector<EPrimitiveFlag> PrimitiveFlagList;
	eastl::vector<AlignedMatrix> LocalToWorldMatrixList;
	eastl::vector<eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>> VertexBufferViewList;
	eastl::vector<D3D12_INDEX_BUFFER_VIEW> IndexBufferViewList;

	/// <summary>
	/// this can be overridden in CreateMeshDrawListForPass
	/// Shader instances of BoundShaderSet will be duplicated for a mesh draw
	/// </summary>
	eastl::vector<FD3D12PSOInitializer::FDrawDesc> DrawDescList;
	eastl::vector<FMeshDrawArgument> MeshDrawArgumentList;
	eastl::array<eastl::vector<FMeshDraw>, static_cast<uint32_t>(EPass::Num)> CachedMeshDrawList;

	void DirtyTransform(const uint32 InPrimitiveIndex);
	void Reserve(const size_t InSize);
};

struct FPrimitive
{
	FPrimitiveList* PrimitiveList;
	uint32_t PrimitiveIndex; // This value represents index in PrimitiveList

	void SetVisible(const bool bInVisible);
	void SetVisible(const EPass InPass, const bool bInVisible);
};

class FRenderScene
{
public:

	void Init();

	EA_NODISCARD FPrimitive AddPrimitive(
		const bool bInVisible,
		const BoundingBox& InLocalBoundingBox, 
		const uint32 InPrimitiveFlag,
		const Matrix& InLocalToWorldMatirx, 
		const float InDrawDistance, 
		const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
		const D3D12_INDEX_BUFFER_VIEW& IndexBufferView,
		const FD3D12PSOInitializer::FDrawDesc& InDrawDesc,
		const FMeshDrawArgument& InMeshDrawArgument
	);

	void CacheMeshDraw(const int32 InPrimitiveIndex);

	// This function will be called from worker thread
	void PrepareToCreateMeshDrawList(FD3D12CommandContext& InCommandContext);
	FMeshDraw MergeMeshDraw(const FMeshDraw& lhs, const FMeshDraw& rhs);
	FMeshDraw CreateMeshDrawForPass(const uint32_t InPrimitiveIndex, const EPass InPass);
	eastl::vector<FMeshDraw> CreateMeshDrawListForPass(const EPass InPass);
	void SetUpShaderInstances(eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList);

	void SetPassDesc(const EPass InPass, const FD3D12PSOInitializer::FPassDesc& InPassDesc);

	FPrimitiveList PrimitiveList;
	GPUScene GPUSceneData;

	struct FPass
	{
		FD3D12PSOInitializer::FPassDesc PassPSODesc;

		eastl::bitvector<> IsCachedMeshDrawList;
	};

private:

	eastl::array<eastl::vector<FMeshDraw>, static_cast<uint32_t>(EPass::Num)> MergedMeshDrawList;

	eastl::vector<uint32> AddedPrimitiveIndexList;
	eastl::array<FPass, static_cast<uint32_t>(EPass::Num)> PerPassData;
};