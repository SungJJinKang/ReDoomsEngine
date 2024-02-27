#include "RenderScene.h"

#include "D3D12CommandContext.h"

static TConsoleVariable<bool> GCacheMeshDraw{ "r.CacheMeshDraw", true };

FRenderObject FRenderScene::AddRenderObject(
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
)
{
	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		RenderObjectList.VisibleFlagsList[PassIndex].push_back(bInVisible);
	}
	RenderObjectList.ModelMatrixDirtyList.push_back(true);
	RenderObjectList.BoundingBoxList.push_back(InLocalBoundingBox);
	RenderObjectList.PositionAndLocalBoundingSphereRadiusList.emplace_back(Position.x, Position.y, Position.z, InLocalBoundingBox.LengthOfCenterToCorner());
	RenderObjectList.RotationList.push_back(InRotation);
	RenderObjectList.ScaleAndDrawDistanceList.emplace_back(InScale.x, InScale.y, InScale.z, InDrawDistance);
	RenderObjectList.CachedModelMatrixList.push_back_uninitialized();
	RenderObjectList.VertexBufferViewList.push_back(InVertexBufferViews);
	RenderObjectList.IndexBufferViewList.push_back(IndexBufferView);
	RenderObjectList.TemplateDrawDescList.push_back(InDrawDesc);
	RenderObjectList.MeshDrawArgumentList.push_back(InMeshDrawArgument);

	FRenderObject NewRenderObject{ &RenderObjectList,  RenderObjectList.ModelMatrixDirtyList.size() - 1 };

	if (GCacheMeshDraw)
	{

	}

	return NewRenderObject;
}

void FRenderScene::PrepareToCreateMeshDrawList()
{
	RenderObjectList.CacheModelMatrixs();
}

eastl::vector<FMeshDraw> FRenderScene::CreateMeshDrawListForPass(const EPass InPass)
{
	SCOPED_CPU_TIMER(FRenderScene_CreateMeshDrawListForPass)
	SCOPED_MEMORY_TRACE(FRenderScene_CreateMeshDrawListForPass)

	eastl::vector<FMeshDraw> MeshDrawList;

	FPass& Pass = PerPassData[static_cast<uint32_t>(InPass)];

	const uint32_t ObjectCount = RenderObjectList.VisibleFlagsList[static_cast<uint32_t>(InPass)].size();
	MeshDrawList.reserve(ObjectCount);

	FD3D12PSOInitializer PSO;
	PSO.PassDesc = Pass.PassPSODesc;

	// Step 1
	// CPU Culling (https://github.com/SungJJinKang/EveryCulling)

	// Step 2
	// Draw!
	for(uint32_t ObjectIndex = 0 ; ObjectIndex < ObjectCount ; ++ObjectIndex)
	{
		const FD3D12PSOInitializer::FDrawDesc& TemplateDrawDesc = RenderObjectList.TemplateDrawDescList[ObjectIndex];
		const FBoundShaderSet& TemplateBoundShaderSet = TemplateDrawDesc.BoundShaderSet;
		
		PSO.DrawDesc = TemplateDrawDesc;

		eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency> DuplicatedShaderInstanceList{};
		for (uint32_t ShaderInstanceIndex = 0; ShaderInstanceIndex < EShaderFrequency::NumShaderFrequency; ++ShaderInstanceIndex)
		{
			if (FD3D12ShaderInstance* TemplateShaderInstance = TemplateBoundShaderSet.GetShaderInstanceList()[ShaderInstanceIndex])
			{
				// Copy shader instance from template
				DuplicatedShaderInstanceList[ShaderInstanceIndex] = TemplateShaderInstance->Duplicate();
			}
		}
		SetUpShaderInstances(ObjectIndex, DuplicatedShaderInstanceList);

		PSO.DrawDesc.BoundShaderSet = FBoundShaderSet{ DuplicatedShaderInstanceList };
		PSO.FinishCreating();

		MeshDrawList.emplace_back(FMeshDraw{ PSO , RenderObjectList.VertexBufferViewList[ObjectIndex], RenderObjectList.IndexBufferViewList[ObjectIndex], RenderObjectList.MeshDrawArgumentList[ObjectIndex]});
	}
	//

	return MeshDrawList;
}

void FRenderScene::SetUpShaderInstances(const uint32_t InObjectIndex, eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList)
{
	for (uint32_t ShaderInstanceIndex = 0; ShaderInstanceIndex < EShaderFrequency::NumShaderFrequency; ++ShaderInstanceIndex)
	{
		if (FD3D12ShaderInstance* ShaderInstance = InShaderInstanceList[ShaderInstanceIndex])
		{
			EA_ASSERT(!(ShaderInstance->bIsTemplateInstance));

			if (ShaderInstanceIndex == EShaderFrequency::Vertex)
			{
				FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate = ShaderInstance->GetShaderParameterContainer();
				const int32_t MeshDrawConstantBufferIndex = ShaderParameterContainerTemplate->GetMeshDrawConstantBufferIndex();
				eastl::vector<FShaderParameterTemplate*>& ShaderParameterList = ShaderParameterContainerTemplate->GetShaderParameterList();
				EA_ASSERT_FORMATTED(
					MeshDrawConstantBufferIndex >= 0 && MeshDrawConstantBufferIndex < ShaderParameterList.size(),
					("Invalid MeshDrawConstantBufferIndex. You need to add ConstantBuffer \"%s\" to Shader \"%s\"", ANSI_TO_WCHAR(MeshDrawConstantBuffer.GetVariableName()), ShaderInstance->GetShaderTemplate()->GetShaderDeclaration().ShaderName)
				);

				static_cast<FConstantBufferTypeMeshDrawConstantBuffer*>(ShaderParameterList[MeshDrawConstantBufferIndex])->MemberVariables.ModelMatrix =
					RenderObjectList.CachedModelMatrixList[InObjectIndex];
			}
		}
	}
}

void FRenderObject::SetVisible(const bool bInVisible)
{
	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		RenderObjectList->VisibleFlagsList[PassIndex][ObjectIndex] = bInVisible;
	}
}

void FRenderObject::SetVisible(const EPass InPass, const bool bInVisible)
{
	RenderObjectList->VisibleFlagsList[static_cast<uint32_t>(InPass)][ObjectIndex] = bInVisible;
}

const DirectX::BoundingBox& FRenderObject::GetBoundingBox() const
{
	return RenderObjectList->BoundingBoxList[ObjectIndex];
}

void FRenderObject::SetBoundingBox(const BoundingBox& InBoundingBox)
{
	RenderObjectList->BoundingBoxList[ObjectIndex] = InBoundingBox;
	RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].z = InBoundingBox.LengthOfCenterToCorner();
}

const DirectX::SimpleMath::Vector3& FRenderObject::GetPosition() const
{
	return reinterpret_cast<const DirectX::SimpleMath::Vector3&>(RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex]);
}

void FRenderObject::SetPosition(const Vector3& InPosition)
{
	RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].x = InPosition.x;
	RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].y = InPosition.y;
	RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].z = InPosition.z;

	RenderObjectList->ModelMatrixDirtyList.set(ObjectIndex, true);
}

float FRenderObject::GetLocalBoundingSphereRadius() const
{
	return RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].z;
}

const DirectX::SimpleMath::Quaternion& FRenderObject::GetRotation() const
{
	return RenderObjectList->RotationList[ObjectIndex];
}

void FRenderObject::SetRotation(const Quaternion& InQuaternion)
{
	RenderObjectList->RotationList[ObjectIndex] = InQuaternion;

	RenderObjectList->ModelMatrixDirtyList.set(ObjectIndex, true);
}

const DirectX::SimpleMath::Vector3& FRenderObject::GetScale() const
{
	return reinterpret_cast<const DirectX::SimpleMath::Vector3&>(RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex]);
}

void FRenderObject::SetScale(const Vector3& InScale)
{
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].x = InScale.x;
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].y = InScale.y;
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].z = InScale.z;

	RenderObjectList->ModelMatrixDirtyList.set(ObjectIndex, true);
}

float FRenderObject::GetDrawDistance() const
{
	return RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].w;
}

void FRenderObject::SetDrawDistance(const float InDrawDistance)
{
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].w = InDrawDistance;
}

void FRenderObjectList::CacheModelMatrixs()
{
	SCOPED_CPU_TIMER(FRenderObjectList_CacheModelMatrixs)
	SCOPED_MEMORY_TRACE(FRenderObjectList_CacheModelMatrixs)

	// todo : multithread?
	for (uint32_t ObjectIndex = 0; ObjectIndex < PositionAndLocalBoundingSphereRadiusList.size(); ++ObjectIndex)
	{
		if (ModelMatrixDirtyList[ObjectIndex])
		{
			const Matrix ModelMatrix = Matrix::CreateTranslation(
				PositionAndLocalBoundingSphereRadiusList[ObjectIndex].x,
				PositionAndLocalBoundingSphereRadiusList[ObjectIndex].y,
				PositionAndLocalBoundingSphereRadiusList[ObjectIndex].z);

			const Matrix RotationMatrix = Matrix::CreateFromQuaternion(RotationList[ObjectIndex]);
			const Matrix ScaleMatrix = Matrix::CreateScale(ScaleAndDrawDistanceList[ObjectIndex].x, ScaleAndDrawDistanceList[ObjectIndex].y, ScaleAndDrawDistanceList[ObjectIndex].z);

			CachedModelMatrixList[ObjectIndex] = ModelMatrix * RotationMatrix * ScaleMatrix;

			ModelMatrixDirtyList[ObjectIndex] = false;
		}
	}
}

void FRenderObjectList::Reserve(const size_t InSize)
{
	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		VisibleFlagsList[PassIndex].reserve(InSize);
	}
	ModelMatrixDirtyList.reserve(InSize);
	BoundingBoxList.reserve(InSize);
	PositionAndLocalBoundingSphereRadiusList.reserve(InSize);
	RotationList.reserve(InSize);
	ScaleAndDrawDistanceList.reserve(InSize);
	CachedModelMatrixList.reserve(InSize);
	VertexBufferViewList.reserve(InSize);
	IndexBufferViewList.reserve(InSize);
	TemplateDrawDescList.reserve(InSize);
	MeshDrawArgumentList.reserve(InSize);
}
