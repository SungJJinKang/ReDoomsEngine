#include "GPUScene.h"

#include "Rendering/RenderScene.h"
#include "D3D12CommandContext.h"
#include "D3D12Resource/D3D12ResourceUploadBatcher.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"

void GPUScene::Init()
{
	GPUSceneBuffer = eastl::make_shared<FD3D12ConstantBufferResource>(GSupportedMaxPrimitiveCount * sizeof(FPrimitiveSceneData), false, nullptr, 0, false, false);
	GPUSceneBuffer->InitResource();
	GPUSceneBuffer->CreateD3D12Resource();
	GPUSceneBuffer->SetDebugNameToResource(EA_WCHAR("GPUSceneBuffer"));
}

void GPUScene::UploadDirtyData(FD3D12CommandContext& InCommandContext, FRenderObjectList& InRenderObjectList)
{
	if (!InRenderObjectList.GPUSceneDirtyObjectList.empty())
	{
		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(InCommandContext, GPUScene_UploadDirtyData);

			FD3D12ResourceUploadBatcher& ResoruceUploadBatcher =
				EA::StdC::Singleton<FD3D12ResourceAllocator>::GetInstance()->ResourceUploadBatcher;

			FD3D12ResourceUpload ResourceUpload{};
			auto SetupResourceUpload = [&ResourceUpload, this]() {
				ResourceUpload.Resource = GPUSceneBuffer->GetResource();
				ResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(ResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));
				ResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(ResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
				};
			SetupResourceUpload();

			eastl::vector<uint8> StagingData{};

			uint32 DirtyObjectCount = 0;
			// @TODO : Implement count function like bitset's
			for (int32 ObjectIndex = 0; ObjectIndex < InRenderObjectList.GPUSceneDirtyObjectList.size(); ++ObjectIndex)
			{
				if (InRenderObjectList.GPUSceneDirtyObjectList[ObjectIndex])
				{
					++DirtyObjectCount;
				}
			}
			StagingData.resize(DirtyObjectCount * sizeof(FPrimitiveSceneData));

			bool bCreateNewSubResourceContainer = true;

			auto TryAddToResoruceUploadBatcher = [&ResourceUpload, &StagingData, &ResoruceUploadBatcher, &SetupResourceUpload](const int32 ObjectIndex) {
				if (ResourceUpload.SubresourceContainers.size() > 0)
				{
					ResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch =
						StagingData.data() + ObjectIndex * sizeof(FPrimitiveSceneData) - reinterpret_cast<const uint8*>(ResourceUpload.SubresourceContainers[0]->SubresourceData.pData);
					ResourceUpload.SubresourceContainers[0]->SubresourceData.SlicePitch = ResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch;

					ResoruceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));
					SetupResourceUpload();
				}
				};

			int32 ObjectIndex = 0;
			for (; ObjectIndex < InRenderObjectList.GPUSceneDirtyObjectList.size(); ++ObjectIndex)
			{
				if (InRenderObjectList.GPUSceneDirtyObjectList[ObjectIndex])
				{
					EA_ASSERT(StagingData.size() > ObjectIndex * sizeof(FPrimitiveSceneData));
					FPrimitiveSceneData* PrimitiveSceneData = reinterpret_cast<FPrimitiveSceneData*>(StagingData.data() + ObjectIndex * sizeof(FPrimitiveSceneData));

					if (bCreateNewSubResourceContainer)
					{
						ResourceUpload.SubresourceContainers.emplace_back(eastl::make_unique<FD3D12SubresourceContainer>());
						ResourceUpload.SubresourceContainers[0]->SubresourceData.pData = PrimitiveSceneData;
						ResourceUpload.SubresourceContainers[0]->DstOffset = ObjectIndex * sizeof(FPrimitiveSceneData);
						bCreateNewSubResourceContainer = false;
					}

					// Fill data

					uint32 VisibilityFlags = 0;
					for (uint32 PassIndex = 0; PassIndex < static_cast<uint32>(EPass::Num); ++PassIndex)
					{
						if (InRenderObjectList.VisibleFlagsList[PassIndex][ObjectIndex])
						{
							VisibilityFlags |= (1 << PassIndex);
						}
					}
					PrimitiveSceneData->Flags = 0;
					PrimitiveSceneData->VisibilityFlags = VisibilityFlags;
					PrimitiveSceneData->LocalToWorld = InRenderObjectList.CachedLocalToWorldMatrixList[ObjectIndex];
					PrimitiveSceneData->WorldToLocal = InRenderObjectList.CachedLocalToWorldMatrixList[ObjectIndex].Invert();

					InRenderObjectList.GPUSceneDirtyObjectList[ObjectIndex] = false;
				}
				else
				{
					TryAddToResoruceUploadBatcher(ObjectIndex);
					bCreateNewSubResourceContainer = true;
				}
			}
			TryAddToResoruceUploadBatcher(ObjectIndex);

			ResoruceUploadBatcher.Flush(InCommandContext);
		}

		InCommandContext.FlushCommandList(ED3D12QueueType::Direct, true);
		InCommandContext.ResetCommandList(ED3D12QueueType::Direct);
		InCommandContext.StateCache.ResetForNewCommandlist();
	
		PrimitiveCount = InRenderObjectList.GPUSceneDirtyObjectList.size();
	}
}

FD3D12ConstantBufferResource* GPUScene::GetGPUSceneBuffer()
{
	return GPUSceneBuffer.get();
}

uint32 GPUScene::GetPrimitiveCount() const
{
	return PrimitiveCount;
}

uint32 GPUScene::GetSceneBufferSize() const
{
	return GPUSceneBuffer->GetBufferSize();
}
