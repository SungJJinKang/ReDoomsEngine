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

struct FRenderObjectList
{
	eastl::array<eastl::bitvector<>, static_cast<uint32_t>(EPass::Num)> VisibleFlagsList;
	eastl::bitvector<> TransformDirtyObjectList;
	eastl::bitvector<> GPUSceneDirtyObjectList;
	eastl::vector<BoundingBox> BoundingBoxList;
	/// <summary>
	/// x, y, z : World position
	/// w : Radius of bounding sphere
	/// </summary>
	eastl::vector<AlignedVector4> PositionAndLocalBoundingSphereRadiusList;
	eastl::vector<AlignedQuaternion> RotationList;
	eastl::vector<AlignedVector4> ScaleAndDrawDistanceList;
	eastl::vector<AlignedMatrix> CachedLocalToWorldMatrixList;
	eastl::vector<eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>> VertexBufferViewList;
	eastl::vector<D3D12_INDEX_BUFFER_VIEW> IndexBufferViewList;

	/// <summary>
	/// this can be overridden in CreateMeshDrawListForPass
	/// Shader instances of BoundShaderSet will be duplicated for a mesh draw
	/// </summary>
	eastl::vector<FD3D12PSOInitializer::FDrawDesc> TemplateDrawDescList;
	eastl::vector<FMeshDrawArgument> MeshDrawArgumentList;

	void CacheLocalToWorldMatrixs();
	void DirtyTransform(const uint32 InObjectIndex);
	void Reserve(const size_t InSize);
};

struct FRenderObject
{
	FRenderObjectList* RenderObjectList;
	uint32_t ObjectIndex; // This value represents index in RenderObjectList

	void SetVisible(const bool bInVisible);
	void SetVisible(const EPass InPass, const bool bInVisible);
	const BoundingBox& GetBoundingBox() const;
	void SetBoundingBox(const BoundingBox& InBoundingBox);
	const Vector3& GetPosition() const;
	void SetPosition(const Vector3& InPosition);
	float GetLocalBoundingSphereRadius() const;
	const Quaternion& GetRotation() const;
	void SetRotation(const Quaternion& InQuaternion);
	const Vector3& GetScale() const;
	void SetScale(const Vector3& InScale);
	float GetDrawDistance() const;
	void SetDrawDistance(const float InDrawDistance);
};

class FRenderScene
{
public:

	void Init();

	EA_NODISCARD FRenderObject AddRenderObject(
		const bool bInVisible,
		const BoundingBox& InLocalBoundingBox, 
		const Vector3& Position, 
		const Quaternion& InRotation, 
		const Vector3& InScale, 
		const float InDrawDistance, 
		const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
		const D3D12_INDEX_BUFFER_VIEW& IndexBufferView,
		const FD3D12PSOInitializer::FDrawDesc& InDrawDesc,
		const FMeshDrawArgument& InMeshDrawArgument
	);

	// This function will be called from worker thread
	void PrepareToCreateMeshDrawList(FD3D12CommandContext& InCommandContext);
	eastl::vector<FMeshDraw> CreateMeshDrawListForPass(const EPass InPass);
	void SetUpShaderInstances(const uint32_t InObjectIndex, eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList);

	void SetPassDesc(const EPass InPass, const FD3D12PSOInitializer::FPassDesc& InPassDesc);

	FRenderObjectList RenderObjectList;
	GPUScene GPUSceneData;

	struct FPass
	{
		FD3D12PSOInitializer::FPassDesc PassPSODesc;

		eastl::bitvector<> IsCachedMeshDrawList;
	};

private:

	eastl::array<FPass, static_cast<uint32_t>(EPass::Num)> PerPassData;

};