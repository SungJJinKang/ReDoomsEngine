#include "RenderScene.h"

#include "D3D12CommandContext.h"
#include "Thread/JobSystem.h"
#include "D3D12Swapchain.h"
#include "D3D12Manager.h"

static TConsoleVariable<bool> GCacheMeshDraw{ "r.CacheMeshDraw", true };

bool CanMergeMeshDraw(const FMeshDraw& InMeshDrawA, const FMeshDraw& InMeshDrawB)
{
	bool bCanMerge = true;

	if(InMeshDrawA.PSO.GetCachedHash() != InMeshDrawB.PSO.GetCachedHash())
	{
		bCanMerge = false;
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
		if(
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

void FRenderScene::Init()
{
	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	GPUSceneData.Init();
}

FPrimitive FRenderScene::AddPrimitive(
	const bool bInVisible,
	const BoundingBox& InLocalBoundingBox, 
	const uint32 InPrimitiveFlag,
	const Matrix& InLocalToWorldMatirx,
	const float InDrawDistance,
	const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
	const D3D12_INDEX_BUFFER_VIEW& IndexBufferView, 
	const FD3D12PSOInitializer::FDrawDesc& InDrawDesc, 
	const FMeshDrawArgument& InMeshDrawArgument
)
{
	EA_ASSERT(GCurrentRendererState == ERendererState::SceneSetup);

	const uint32 PrimitiveIndex = PrimitiveList.TransformDirtyPrimitiveList.size();

	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		PrimitiveList.VisibleFlagsList[PassIndex].push_back(bInVisible);
		PrimitiveList.CachedMeshDrawList[PassIndex].emplace_back();
	}
	PrimitiveList.TransformDirtyPrimitiveList.push_back(true);
	PrimitiveList.GPUSceneDirtyPrimitiveList.push_back(true);

	BoundingBox TransformedBoundingBox{};
	InLocalBoundingBox.Transform(TransformedBoundingBox, InLocalToWorldMatirx);
	PrimitiveList.WorldSpaceBoundingBoxList.push_back(TransformedBoundingBox);

	PrimitiveList.PrimitiveFlagList.push_back(static_cast<EPrimitiveFlag>(InPrimitiveFlag));
	PrimitiveList.LocalToWorldMatrixList.push_back(InLocalToWorldMatirx);
	PrimitiveList.VertexBufferViewList.push_back(InVertexBufferViews);
	PrimitiveList.IndexBufferViewList.push_back(IndexBufferView);
	PrimitiveList.DrawDescList.push_back(InDrawDesc);
	PrimitiveList.MeshDrawArgumentList.push_back(InMeshDrawArgument);
	
	FPrimitive NewPrimitive{ &PrimitiveList,  PrimitiveIndex };

	if (InPrimitiveFlag & EPrimitiveFlag::CacheMeshDrawCommand)
	{
		if (GCacheMeshDraw)
		{
			CacheMeshDraw(PrimitiveIndex);
		}
	}

	return NewPrimitive;
}

void FRenderScene::CacheMeshDraw(const int32 InPrimitiveIndex)
{
	for (uint32 PassIndex = 0 ; PassIndex < static_cast<uint32>(EPass::Num) ; ++PassIndex)
	{
		FMeshDraw MeshDraw = CreateMeshDrawForPass(InPrimitiveIndex, static_cast<EPass>(PassIndex));
		PrimitiveList.CachedMeshDrawList[PassIndex][InPrimitiveIndex] = MeshDraw;
	}
}

void FRenderScene::PrepareToCreateMeshDrawList(FD3D12CommandContext& InCommandContext)
{
	SCOPED_CPU_TIMER(FRenderScene_PrepareToCreateMeshDrawList)

	GPUSceneData.UploadDirtyData(InCommandContext, PrimitiveList);
}

FMeshDraw FRenderScene::MergeMeshDraw(const FMeshDraw& lhs, const FMeshDraw& rhs)
{
	EA_ASSERT(CanMergeMeshDraw(lhs, rhs));
	EA_ASSERT(lhs.MeshDrawArgument.InstanceCount == lhs.PrimitiveIdList.size());
	EA_ASSERT(rhs.MeshDrawArgument.InstanceCount == rhs.PrimitiveIdList.size());

	FMeshDraw MergedMeshDraw = lhs;

	MergedMeshDraw.MeshDrawArgument.InstanceCount += rhs.MeshDrawArgument.InstanceCount;
	MergedMeshDraw.PrimitiveIdList = lhs.PrimitiveIdList;
    for (uint32_t PrimitiveId : rhs.PrimitiveIdList)
    {
        MergedMeshDraw.PrimitiveIdList.push_back(PrimitiveId);
    }

	return MergedMeshDraw;
}

FMeshDraw FRenderScene::CreateMeshDrawForPass(const uint32_t InPrimitiveIndex, const EPass InPass)
{
	FMeshDraw MeshDraw{};

	MeshDraw.VertexBufferViewList = PrimitiveList.VertexBufferViewList[InPrimitiveIndex];
	MeshDraw.IndexBufferView = PrimitiveList.IndexBufferViewList[InPrimitiveIndex];
	MeshDraw.MeshDrawArgument = PrimitiveList.MeshDrawArgumentList[InPrimitiveIndex];

	const FD3D12PSOInitializer::FDrawDesc& DrawDesc = PrimitiveList.DrawDescList[InPrimitiveIndex];

	FD3D12PSOInitializer PSO;
	PSO.PassDesc = PerPassData[static_cast<uint32_t>(InPass)].PassPSODesc;
	PSO.DrawDesc = DrawDesc;
	MeshDraw.PSO = PSO;

	eastl::fixed_vector<uint32, 1> PrimitiveIdList;
	PrimitiveIdList.emplace_back(InPrimitiveIndex);
	MeshDraw.PrimitiveIdList = PrimitiveIdList;
	
	return MeshDraw;
}

eastl::vector<FMeshDraw> FRenderScene::CreateMeshDrawListForPass(const EPass InPass)
{
	SCOPED_CPU_TIMER(FRenderScene_CreateMeshDrawListForPass)
	SCOPED_MEMORY_TRACE(FRenderScene_CreateMeshDrawListForPass)

	eastl::vector<FMeshDraw> MeshDrawList{};

	FPass& Pass = PerPassData[static_cast<uint32_t>(InPass)];

	const uint32_t PrimitiveCount = PrimitiveList.VisibleFlagsList[static_cast<uint32_t>(InPass)].size();
	
	// Draw!
	for(uint32_t PrimitiveIndex = 0 ; PrimitiveIndex < PrimitiveCount ; ++PrimitiveIndex)
	{
		FMeshDraw NewMeshDraw{};

		if (GCacheMeshDraw && (PrimitiveList.PrimitiveFlagList[PrimitiveIndex] & EPrimitiveFlag::CacheMeshDrawCommand))
		{
			NewMeshDraw = PrimitiveList.CachedMeshDrawList[static_cast<uint32_t>(InPass)][PrimitiveIndex];
		}
		else
		{
			NewMeshDraw = CreateMeshDrawForPass(PrimitiveIndex, InPass);
			NewMeshDraw.bIsValid = true;
		}
		NewMeshDraw.PSO.PassDesc = Pass.PassPSODesc;

		bool bMerged = false;

		for (FMeshDraw& MeshDraw : MeshDrawList)
		{
			if (CanMergeMeshDraw(MeshDraw, NewMeshDraw))
			{
				MeshDraw = MergeMeshDraw(MeshDraw, NewMeshDraw);
				bMerged = true;
				break;
			}
		}

		if (!bMerged)
		{
			MeshDrawList.push_back(NewMeshDraw);
		}
	}
	//

	for (FMeshDraw& MeshDraw : MeshDrawList)
	{
		SetUpShaderInstances(MeshDraw.PSO.DrawDesc.BoundShaderSet.GetShaderInstanceList());
	}

	return MeshDrawList;
}


void FRenderScene::SetUpShaderInstances(eastl::array<FD3D12ShaderInstance*, EShaderFrequency::NumShaderFrequency>& InShaderInstanceList)
{
	SCOPED_CPU_TIMER(FRenderScene_SetUpShaderInstances)

	for (uint32_t ShaderInstanceIndex = 0; ShaderInstanceIndex < EShaderFrequency::NumShaderFrequency; ++ShaderInstanceIndex)
	{
		if (FD3D12ShaderInstance* ShaderInstance = InShaderInstanceList[ShaderInstanceIndex])
		{
			//EA_ASSERT(!(ShaderInstance->bIsTemplateInstance));

			FShaderParameterContainerTemplate* const ShaderParameterContainerTemplate = ShaderInstance->GetShaderParameterContainer();
			eastl::vector<FShaderParameterTemplate*>& ShaderParameterList = ShaderParameterContainerTemplate->GetShaderParameterList();
		
			const int32_t PrimitiveSceneDataSRVIndex = ShaderParameterContainerTemplate->GetPrimitiveSceneDataSRVIndex();
			if (PrimitiveSceneDataSRVIndex != -1)
			{
				FD3D12SRVDesc PrimitiveSceneDataSRV{};
				PrimitiveSceneDataSRV.ShaderParameterResourceType = EShaderParameterResourceType::StructuredBuffer;
				PrimitiveSceneDataSRV.StructureByteStride = 16;
				PrimitiveSceneDataSRV.NumElements = GPUSceneData.GetSceneBufferSize() / PrimitiveSceneDataSRV.StructureByteStride;

				*static_cast<FShaderParameterShaderResourceView*>(ShaderParameterList[PrimitiveSceneDataSRVIndex]) = GPUSceneData.GetGPUSceneBuffer()->GetSRV(PrimitiveSceneDataSRV);
			}
		}
	}
}

void FRenderScene::SetPassDesc(const EPass InPass, const FD3D12PSOInitializer::FPassDesc& InPassDesc)
{
	PerPassData[static_cast<uint32_t>(InPass)].PassPSODesc = InPassDesc;

	// @todo invalidate IsCachedMeshDrawList
}

void FPrimitive::SetVisible(const bool bInVisible)
{
	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		PrimitiveList->VisibleFlagsList[PassIndex][PrimitiveIndex] = bInVisible;
	}
}

void FPrimitive::SetVisible(const EPass InPass, const bool bInVisible)
{
	PrimitiveList->VisibleFlagsList[static_cast<uint32_t>(InPass)][PrimitiveIndex] = bInVisible;
}

void FPrimitiveList::DirtyTransform(const uint32 InPrimitiveIndex)
{
	TransformDirtyPrimitiveList.set(InPrimitiveIndex, true);
	GPUSceneDirtyPrimitiveList.set(InPrimitiveIndex, true);
}

void FPrimitiveList::Reserve(const size_t InSize)
{
	for (uint32_t PassIndex = 0; PassIndex < static_cast<uint32_t>(EPass::Num); ++PassIndex)
	{
		VisibleFlagsList[PassIndex].reserve(InSize);
	}
	TransformDirtyPrimitiveList.reserve(InSize);
	WorldSpaceBoundingBoxList.reserve(InSize);
	PrimitiveFlagList.reserve(InSize);
	LocalToWorldMatrixList.reserve(InSize);
	VertexBufferViewList.reserve(InSize);
	IndexBufferViewList.reserve(InSize);
	DrawDescList.reserve(InSize);
	MeshDrawArgumentList.reserve(InSize);
}
