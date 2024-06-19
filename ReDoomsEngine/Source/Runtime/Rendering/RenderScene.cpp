#include "RenderScene.h"

#include "D3D12CommandContext.h"
#include "Thread/JobSystem.h"
#include "D3D12Swapchain.h"
#include "D3D12Manager.h"
#include "View.h"

#include "EveryCulling/DataType/EntityBlock.h"

static TConsoleVariable<bool> GCacheMeshDraw{ "r.CacheMeshDraw", true };
static TConsoleVariable<bool> GParallelCacheModelMatrixs{ "r.ParallelCacheModelMatrixs", true };
static TConsoleVariable<uint32_t> GParallelCacheModelMatrixsObjectCountPerJob{ "r.ParallelCacheModelMatrixsObjectCountPerJob", 512 };

static TConsoleVariable<bool> GEveryCullingEnable{ "r.EveryCulling.Enable", true };
static TConsoleVariable<bool> GEveryCullingParallel{ "r.EveryCulling.Parallel", true };
static TConsoleVariable<bool> GEveryCullingPreCulling{ "r.EveryCulling.PreCulling", true, EConsoleVariableFlag::HideInEditor };
static TConsoleVariable<bool> GEveryCullingDistanceCulling{ "r.EveryCulling.DistanceCulling", true };
static TConsoleVariable<bool> GEveryCullingViewFrustumCulling{ "r.EveryCulling.ViewFrustumCulling", true };

// Why this is required? : In EveryCulling library, use its own math type instead of DirectXMath's. So we should ensure that our math types have same size and alignment with matching ones.
static_assert(sizeof(culling::AlignedVec4) == sizeof(math::Vector4));
static_assert(alignof(culling::AlignedVec4) == alignof (math::Vector4));
static_assert(sizeof(culling::Vec3) == sizeof(math::Vector3));
static_assert(alignof(culling::Vec3) == alignof (math::Vector3));
static_assert(sizeof(culling::Vec2) == sizeof(math::Vector2));
static_assert(alignof(culling::Vec2) == alignof (math::Vector2));
static_assert(sizeof(culling::Position_BoundingSphereRadius) == sizeof(math::Vector4));
static_assert(alignof(culling::Position_BoundingSphereRadius) == alignof (math::Vector4));
static_assert(sizeof(culling::Mat4x4) == sizeof(math::Matrix4x4));
static_assert(alignof(culling::Mat4x4) == alignof (math::Matrix4x4));

void FRenderScene::Init()
{
	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	EveryCulling = eastl::make_unique<culling::EveryCulling>(SwapChain->GetWidth(), SwapChain->GetHeight());
	EveryCulling->SetCameraCount(1);
}

FRenderObject FRenderScene::AddRenderObject(
	const bool bInVisible,
	const math::Vector3& InLocalPositionAABBMinPoint,
	const math::Vector3& InLocalPositionAABBMaxPoint,
	const math::Vector3& Position,
	const math::Quaternion& InRotation,
	const math::Vector3& InScale,
	const float InDrawDistance,
	const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
	const D3D12_INDEX_BUFFER_VIEW& IndexBufferView, 
	const FD3D12PSOInitializer::FDrawDesc& InDrawDesc, 
	const FMeshDrawArgument& InMeshDrawArgument
)
{
// 	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
// 	{
// 		RenderObjectList.EnabledFlagsList[PassIndex].push_back(bInVisible);
// 	}
	RenderObjectList.EnabledFlagsList.push_back(bInVisible);
	RenderObjectList.VisibleFlagsList.push_back(0xFF);
	RenderObjectList.TransformDirtyList.push_back(true);
	RenderObjectList.LocalPositionAABBMinPointList.push_back(InLocalPositionAABBMinPoint);
	RenderObjectList.LocalPositionAABBMaxPointList.push_back(InLocalPositionAABBMaxPoint);
	RenderObjectList.WorldPositionAABBMinPointList.push_back_uninitialized();
	RenderObjectList.WorldPositionAABBMaxPointList.push_back_uninitialized();
	RenderObjectList.LocalBoundingSphereRadiusList.emplace_back((InLocalPositionAABBMaxPoint - InLocalPositionAABBMinPoint).magnitude() * 0.5f);
	RenderObjectList.PositionAndWorldBoundingSphereRadiusList.emplace_back(Position.x, Position.y, Position.z, -1);
	RenderObjectList.RotationList.push_back(InRotation);
	RenderObjectList.ScaleAndDrawDistanceList.emplace_back(InScale.x, InScale.y, InScale.z, InDrawDistance);
	RenderObjectList.CachedModelMatrixList.push_back_uninitialized();
	RenderObjectList.VertexBufferViewList.push_back(InVertexBufferViews);
	RenderObjectList.IndexBufferViewList.push_back(IndexBufferView);
	EA_ASSERT(InDrawDesc.IsValidHash());
	RenderObjectList.TemplateDrawDescList.push_back(InDrawDesc);
	RenderObjectList.MeshDrawArgumentList.push_back(InMeshDrawArgument);

	FRenderObject NewRenderObject{ &RenderObjectList,  RenderObjectList.TransformDirtyList.size() - 1 };

	if (GCacheMeshDraw)
	{
		// @todo : Implement

	}

	return NewRenderObject;
}

eastl::vector<culling::EntityBlock> FRenderScene::CreateEveryCullingEntityBlockList()
{
	SCOPED_CPU_TIMER(FRenderScene_CreateEveryCullingEntityBlockList)
	SCOPED_MEMORY_TRACE(FRenderScene_CreateEveryCullingEntityBlockList)

	eastl::vector<culling::EntityBlock> EveryCullingEntityBlockList{};

	const uint32_t ObjectCount = RenderObjectList.GetObjectCount();

	const uint32_t EntityBlockCount = (ObjectCount + EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK - 1) / EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK;
	EveryCullingEntityBlockList.resize(EntityBlockCount); // @todo : implement uninitialized resize

	// Never parallel this code. it's already cheap.

	for(uint32_t EntityBlockIndex = 0 ; EntityBlockIndex < EntityBlockCount ; ++EntityBlockIndex)
	{
		int32_t StartIndex = EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK * EntityBlockIndex;
		int32_t EndIndex = EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK * EntityBlockIndex + EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK;
		culling::EntityBlock& TargetEntityBlock = EveryCullingEntityBlockList[EntityBlockIndex];
		
		TargetEntityBlock.mIsVisibleBitflag = RenderObjectList.VisibleFlagsList.data() + StartIndex;
		TargetEntityBlock.mAABBMinWorldPoint = reinterpret_cast<culling::AlignedVec4*>(RenderObjectList.WorldPositionAABBMinPointList.data() + StartIndex);
		TargetEntityBlock.mAABBMaxWorldPoint = reinterpret_cast<culling::AlignedVec4*>(RenderObjectList.WorldPositionAABBMaxPointList.data() + StartIndex);
		TargetEntityBlock.mIsObjectEnabled = reinterpret_cast<uint8_t*>(RenderObjectList.EnabledFlagsList.data()) + (StartIndex / 8);
		TargetEntityBlock.mScaleAndDrawDistance = reinterpret_cast<culling::AlignedVec4*>(RenderObjectList.ScaleAndDrawDistanceList.data() + StartIndex);
		TargetEntityBlock.mWorldPositionAndWorldBoundingSphereRadius = reinterpret_cast<culling::Position_BoundingSphereRadius*>(RenderObjectList.PositionAndWorldBoundingSphereRadiusList.data() + StartIndex);
		TargetEntityBlock.EntityCount = (EndIndex <= ObjectCount) ? EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK : (EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK - (EndIndex - ObjectCount));
	};

	return EveryCullingEntityBlockList;
}

void FRenderScene::PrepareToCreateMeshDrawList(const FView& InView)
{
	SCOPED_CPU_TIMER(FRenderScene_PrepareToCreateMeshDrawList)
	SCOPED_MEMORY_TRACE(FRenderScene_PrepareToCreateMeshDrawList)

	RenderObjectList.CacheModelMatrixs();

	// CPU Culling (https://github.com/SungJJinKang/EveryCulling)

	if (GEveryCullingEnable)
	{
		SCOPED_CPU_TIMER(FRenderScene_PrepareToCreateMeshDrawList_PrepareEveryCullingData)
		SCOPED_MEMORY_TRACE(FRenderScene_PrepareToCreateMeshDrawList_PrepareEveryCullingData)

		culling::EveryCulling::GlobalDataForCullJob GlobalDataForCullJob;
		const math::Matrix4x4 ViewPerspectiveProjectionMatrix = InView.GetViewPerspectiveProjectionMatrix(true);
		GlobalDataForCullJob.mViewProjectionMatrix = *reinterpret_cast<const culling::Mat4x4*>(&ViewPerspectiveProjectionMatrix);
		GlobalDataForCullJob.mFieldOfViewInDegree = InView.FovInDegree;
		GlobalDataForCullJob.mCameraNearPlaneDistance = InView.NearPlane;
		GlobalDataForCullJob.mCameraFarPlaneDistance = InView.FarPlane;
		GlobalDataForCullJob.mCameraWorldPosition = *reinterpret_cast<const culling::Vec3*>(&InView.Transform.Position);
		const math::Vector4 Rot = static_cast<math::Vector4>(InView.Transform.Rotation);
		GlobalDataForCullJob.mCameraRotation = *reinterpret_cast<const culling::AlignedVec4*>(&Rot);

		EveryCulling->SetCameraCount(1);
		{
			SCOPED_CPU_TIMER(EveryCulling_UpdateGlobalDataForCullJob)
			EveryCulling->UpdateGlobalDataForCullJob(0, GlobalDataForCullJob);
		}
		EveryCulling->mEntityBlockList = CreateEveryCullingEntityBlockList();

		EveryCulling->SetEnabledCullingModule(culling::EveryCulling::CullingModuleType::PreCulling, GEveryCullingPreCulling);
		EveryCulling->SetEnabledCullingModule(culling::EveryCulling::CullingModuleType::DistanceCulling, GEveryCullingDistanceCulling);
		EveryCulling->SetEnabledCullingModule(culling::EveryCulling::CullingModuleType::ViewFrustumCulling, GEveryCullingViewFrustumCulling);
		{
			SCOPED_CPU_TIMER(EveryCulling_PreCullJob)
			EveryCulling->PreCullJob();
		}
		const unsigned long long CurrentTickCount = EveryCulling->GetTickCount();

		{
			SCOPED_CPU_TIMER(EveryCulling_DispatchCullJob)
			if (GEveryCullingParallel)
			{
				FJobSystem::GetInstance()->Dispatch(FJobSystem::GetInstance()->GetJobThreadCount() + 1,
					[this, CurrentTickCount](FJobDispatchArgs DispatchArg) {
						EveryCulling->ThreadCullJob(0, CurrentTickCount);
					},
					true);
			}
			else
			{
				EveryCulling->ThreadCullJob(0, CurrentTickCount);
			}
		}
	}
	else
	{
		EA::StdC::Memset8(RenderObjectList.VisibleFlagsList.data(), 0xFF, RenderObjectList.VisibleFlagsList.size());
	}
}

eastl::vector<FMeshDraw> FRenderScene::CreateMeshDrawListForPass(const EPass InPass)
{
	SCOPED_CPU_TIMER(FRenderScene_CreateMeshDrawListForPass)
	SCOPED_MEMORY_TRACE(FRenderScene_CreateMeshDrawListForPass)

	eastl::vector<FMeshDraw> MeshDrawList;

	FPass& Pass = PerPassData[static_cast<uint32_t>(InPass)];

	const uint32_t ObjectCount = RenderObjectList.VisibleFlagsList.size();
	MeshDrawList.reserve(ObjectCount);

	FD3D12PSOInitializer PSO;
	PSO.PassDesc = Pass.PassPSODesc;

	// Draw!
	for(uint32_t ObjectIndex = 0 ; ObjectIndex < ObjectCount ; ++ObjectIndex)
	{
		if ((RenderObjectList.VisibleFlagsList[ObjectIndex] & 1) != 0)
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

			{
				SCOPED_CPU_TIMER(FRenderScene_CreateMeshDrawListForPass_PSOSetup)
					PSO.DrawDesc.BoundShaderSet = FBoundShaderSet{ DuplicatedShaderInstanceList };
				PSO.CacheHash();
			}

			MeshDrawList.emplace_back(FMeshDraw{ PSO , RenderObjectList.VertexBufferViewList[ObjectIndex], RenderObjectList.IndexBufferViewList[ObjectIndex], RenderObjectList.MeshDrawArgumentList[ObjectIndex] });
		}
		else
		{

		}
	}
	//

	return MeshDrawList;
}

void FRenderScene::SetUpShaderInstances(const uint32_t InObjectIndex, eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList)
{
	SCOPED_CPU_TIMER(FRenderScene_SetUpShaderInstances)

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

void FRenderScene::SetPassDesc(const EPass InPass, const FD3D12PSOInitializer::FPassDesc& InPassDesc)
{
	EA_ASSERT(InPassDesc.IsValidHash());
	PerPassData[static_cast<uint32_t>(InPass)].PassPSODesc = InPassDesc;

	// @todo invalidate IsCachedMeshDrawList
}

void FRenderObject::SetVisible(const bool bInVisible)
{
// 	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
// 	{
// 		RenderObjectList->EnabledFlagsList[PassIndex][ObjectIndex] = bInVisible;
// 	}

	RenderObjectList->EnabledFlagsList[ObjectIndex] = bInVisible;
}

void FRenderObject::SetVisible(const EPass InPass, const bool bInVisible)
{
	/*RenderObjectList->EnabledFlagsList[static_cast<uint32_t>(InPass)][ObjectIndex] = bInVisible;*/
	RenderObjectList->EnabledFlagsList[ObjectIndex] = bInVisible;
}

// const DirectX::BoundingBox& FRenderObject::GetBoundingBox() const
// {
// 	return RenderObjectList->BoundingBoxList[ObjectIndex];
// }
// 
// void FRenderObject::SetBoundingBox(const BoundingBox& InBoundingBox)
// {
// 	RenderObjectList->BoundingBoxList[ObjectIndex] = InBoundingBox;
// 	RenderObjectList->PositionAndLocalBoundingSphereRadiusList[ObjectIndex].z = InBoundingBox.LengthOfCenterToCorner();
// }

const math::Vector3& FRenderObject::GetPosition() const
{
	return reinterpret_cast<const math::Vector3&>(RenderObjectList->PositionAndWorldBoundingSphereRadiusList[ObjectIndex]);
}

void FRenderObject::SetPosition(const math::Vector3& InPosition)
{
	RenderObjectList->PositionAndWorldBoundingSphereRadiusList[ObjectIndex].x = InPosition.x;
	RenderObjectList->PositionAndWorldBoundingSphereRadiusList[ObjectIndex].y = InPosition.y;
	RenderObjectList->PositionAndWorldBoundingSphereRadiusList[ObjectIndex].z = InPosition.z;

	RenderObjectList->TransformDirtyList.set(ObjectIndex, true);
}

// float FRenderObject::GetLocalBoundingSphereRadius() const
// {
// 	return RenderObjectList->PositionAndWorldBoundingSphereRadiusList[ObjectIndex].z;
// }

const math::Quaternion& FRenderObject::GetRotation() const
{
	return RenderObjectList->RotationList[ObjectIndex];
}

void FRenderObject::SetRotation(const math::Quaternion& InQuaternion)
{
	RenderObjectList->RotationList[ObjectIndex] = InQuaternion;

	RenderObjectList->TransformDirtyList.set(ObjectIndex, true);
}

const math::Vector3& FRenderObject::GetScale() const
{
	return reinterpret_cast<const math::Vector3&>(RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex]);
}

void FRenderObject::SetScale(const math::Vector3& InScale)
{
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].x = InScale.x;
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].y = InScale.y;
	RenderObjectList->ScaleAndDrawDistanceList[ObjectIndex].z = InScale.z;

	RenderObjectList->TransformDirtyList.set(ObjectIndex, true);
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

	bool ParallelCacheModelMatrixs = GParallelCacheModelMatrixs;
	const uint32_t ObjectCountPerJob = GParallelCacheModelMatrixsObjectCountPerJob;
	const uint32_t ObjectCount = GetObjectCount();
	auto CacheModelMatrixsJob = [this, ObjectCountPerJob, ObjectCount](const int32_t JobIndex)
	{
		int32_t StartIndex = ObjectCountPerJob * JobIndex;
		int32_t EndIndex = ObjectCountPerJob * JobIndex + ObjectCountPerJob;
		for (uint32_t ObjectIndex = StartIndex; (ObjectIndex < EndIndex) && (ObjectIndex < ObjectCount); ++ObjectIndex)
		{
			if (TransformDirtyList[ObjectIndex])
			{
				const math::Matrix4x4 TranslationMatrix = math::translate(
					PositionAndWorldBoundingSphereRadiusList[ObjectIndex].x,
					PositionAndWorldBoundingSphereRadiusList[ObjectIndex].y,
					PositionAndWorldBoundingSphereRadiusList[ObjectIndex].z);

				const math::Matrix4x4 RotationMatrix = static_cast<math::Matrix4x4>(math::Quaternion{1.0f, 0.0f, 0.0f, 0.0f});
				const math::Matrix4x4 ScaleMatrix = math::scale(ScaleAndDrawDistanceList[ObjectIndex].x, ScaleAndDrawDistanceList[ObjectIndex].y, ScaleAndDrawDistanceList[ObjectIndex].z);

				CachedModelMatrixList[ObjectIndex] = TranslationMatrix * RotationMatrix * ScaleMatrix;
				
				WorldPositionAABBMinPointList[ObjectIndex] = TranslationMatrix * LocalPositionAABBMinPointList[ObjectIndex];
				WorldPositionAABBMinPointList[ObjectIndex] = TranslationMatrix * LocalPositionAABBMinPointList[ObjectIndex];
				WorldPositionAABBMaxPointList[ObjectIndex] = TranslationMatrix * LocalPositionAABBMaxPointList[ObjectIndex];
				PositionAndWorldBoundingSphereRadiusList[ObjectIndex].w = (WorldPositionAABBMaxPointList[ObjectIndex] - WorldPositionAABBMinPointList[ObjectIndex]).magnitude() * 0.5f;
				EA_ASSERT(PositionAndWorldBoundingSphereRadiusList[ObjectIndex].w > 0);

				TransformDirtyList[ObjectIndex] = false;
			}
		}
	};

	const uint32_t JobCount = (ObjectCount + ObjectCountPerJob - 1) / ObjectCountPerJob;
	if (JobCount < 3) // if job count is too small, just process it on current thread
	{
		ParallelCacheModelMatrixs = false;
	}

	if (ParallelCacheModelMatrixs)
	{
		FJobSystem::GetInstance()->Dispatch(JobCount, [CacheModelMatrixsJob](FJobDispatchArgs DispatchArg) {CacheModelMatrixsJob(DispatchArg.JobIndex); }, true);
	}
	else
	{
		for (uint32_t JobIndex = 0; JobIndex < JobCount; ++JobIndex)
		{
			CacheModelMatrixsJob(JobIndex);
		}
	}
	
}

void FRenderObjectList::Reserve(const size_t InSize)
{
// 	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
// 	{
// 		EnabledFlagsList[PassIndex].reserve(InSize);
// 	}
	EnabledFlagsList.reserve(InSize);
	VisibleFlagsList.reserve(InSize);
	TransformDirtyList.reserve(InSize);
	LocalPositionAABBMinPointList.reserve(InSize);
	LocalPositionAABBMaxPointList.reserve(InSize);
	WorldPositionAABBMinPointList.reserve(InSize);
	WorldPositionAABBMaxPointList.reserve(InSize);
	LocalBoundingSphereRadiusList.reserve(InSize);
	PositionAndWorldBoundingSphereRadiusList.reserve(InSize);
	RotationList.reserve(InSize);
	ScaleAndDrawDistanceList.reserve(InSize);
	CachedModelMatrixList.reserve(InSize);
	VertexBufferViewList.reserve(InSize);
	IndexBufferViewList.reserve(InSize);
	TemplateDrawDescList.reserve(InSize);
	MeshDrawArgumentList.reserve(InSize);
}

uint32_t FRenderObjectList::GetObjectCount() const
{
	return TransformDirtyList.size();
}
