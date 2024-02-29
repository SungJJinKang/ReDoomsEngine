#pragma once

#include "CommonInclude.h"
#include "D3D12PSO.h"
#include "EASTL/segmented_vector.h"
#include "MeshDraw/MeshDraw.h"
#include "D3D12Shader.h"
#include "EveryCulling/EveryCulling.h"
#include "EveryCulling/DataType/Position_BoundingSphereRadius.h"
#include "EveryCulling/DataType/VertexData.h"

struct FMesh;
struct FD3D12CommandContext;
class FView;

enum class EPass : uint32_t
{
	BasePass,

	Num
};

struct FRenderObjectList
{
	eastl::bitvector<> EnabledFlagsList;
	//eastl::array<eastl::bitvector<>, static_cast<uint32_t>(EPass::Num)> EnabledFlagsList;
	/// <summary>
	/// Each bit represents if the object is visible to each camera
	/// </summary>
	eastl::vector<uint8_t> VisibleFlagsList;
	eastl::bitvector<> TransformDirtyList;
	eastl::vector<Vector3> LocalPositionAABBMinPointList;
	eastl::vector<Vector3> LocalPositionAABBMaxPointList;
	eastl::vector<AlignedVector3> WorldPositionAABBMinPointList;
	eastl::vector<AlignedVector3> WorldPositionAABBMaxPointList;


	eastl::vector<float> LocalBoundingSphereRadiusList;
	/// <summary>
	/// x, y, z : World position
	/// w : Radius of bounding sphere
	/// </summary>
	eastl::vector<AlignedVector4> PositionAndWorldBoundingSphereRadiusList;

	eastl::vector<AlignedQuaternion> RotationList;
	eastl::vector<AlignedVector4> ScaleAndDrawDistanceList;
	eastl::vector<AlignedMatrix> CachedModelMatrixList;
	eastl::vector<AlignedMatrix> CachedColumnMajorModelMatrixList;
	eastl::vector<culling::VertexData> EveryCullingVertexData;
	eastl::vector<eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>> VertexBufferViewList;
	eastl::vector<D3D12_INDEX_BUFFER_VIEW> IndexBufferViewList;

	/// <summary>
	/// this can be overridden in CreateMeshDrawListForPass
	/// Shader instances of BoundShaderSet will be duplicated for a mesh draw
	/// </summary>
	eastl::vector<FD3D12PSOInitializer::FDrawDesc> TemplateDrawDescList;
	eastl::vector<FMeshDrawArgument> MeshDrawArgumentList;

	void CacheModelMatrixs();
	void Reserve(const size_t InSize);
	uint32_t GetObjectCount() const;
};

struct FRenderObject
{
	FRenderObjectList* RenderObjectList;
	uint32_t ObjectIndex; // This value represents index in RenderObjectList

	void SetVisible(const bool bInVisible);
	void SetVisible(const EPass InPass, const bool bInVisible);
// 	const BoundingBox& GetBoundingBox() const;
// 	void SetBoundingBox(const BoundingBox& InBoundingBox);
	const Vector3& GetPosition() const;
	void SetPosition(const Vector3& InPosition);
/*	float GetLocalBoundingSphereRadius() const;*/
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
		const Vector3& InLocalPositionAABBMinPoint,
		const Vector3& InLocalPositionAABBMaxPoint,
		const Vector3& Position, 
		const Quaternion& InRotation, 
		const Vector3& InScale, 
		const culling::VertexData& InEveryCullingVertexData,
		const float InDrawDistance, 
		const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
		const D3D12_INDEX_BUFFER_VIEW& IndexBufferView,
		const FD3D12PSOInitializer::FDrawDesc& InDrawDesc,
		const FMeshDrawArgument& InMeshDrawArgument
	);

	eastl::vector<culling::EntityBlock> CreateEveryCullingEntityBlockList();
	// This function will be called from worker thread
	void PrepareToCreateMeshDrawList(const FView& InView);
	eastl::vector<FMeshDraw> CreateMeshDrawListForPass(const EPass InPass);
	void SetUpShaderInstances(const uint32_t InObjectIndex, eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList);

	void SetPassDesc(const EPass InPass, const FD3D12PSOInitializer::FPassDesc& InPassDesc);

	FRenderObjectList RenderObjectList;
	eastl::unique_ptr<culling::EveryCulling> EveryCulling;

	struct FPass
	{
		FD3D12PSOInitializer::FPassDesc PassPSODesc;

		eastl::bitvector<> IsCachedMeshDrawList;
	};

private:

	eastl::array<FPass, static_cast<uint32_t>(EPass::Num)> PerPassData;

};

DEFINE_SHADER_CONSTANT_BUFFER_TYPE(
	MeshDrawConstantBuffer, true,
	ADD_SHADER_CONSTANT_BUFFER_MEMBER_VARIABLE(Matrix, ModelMatrix)
)