#include "GPUScene.h"

#include "Rendering/RenderScene.h"

void GPUScene::Init()
{
	GPUSceneBuffer = eastl::make_unique<FD3D12ConstantBufferResource>(GSupportedMaxPrimitiveCount * sizeof(FPrimitiveSceneData), false);
}

void GPUScene::UploadDirtyData(FD3D12CommandContext& InCommandContext, const FRenderObjectList& InRenderObjectList, const eastl::bitvector<>& InDirtyObjectList)
{
	if (!InDirtyObjectList.empty())
	{
		FD3D12ResourceUploadBatcher& ResoruceUploadBatcher =
			EA::StdC::Singleton<FD3D12ResourceAllocator>::GetInstance()->ResourceUploadBatcher;

		FD3D12ResourceUpload ResourceUpload{};
		ResourceUpload.Resource = GPUSceneBuffer->GetResource();

		eastl::vector<uint8> StagingData{};
		
		uint32 DirtyObjectCount = 0;
		// @TODO : Implement count function like bitset's
		for (int32 ObjectIndex = 0; ObjectIndex < InDirtyObjectList.size(); ++ObjectIndex)
		{
			if (InDirtyObjectList[ObjectIndex])
			{
				++DirtyObjectCount;
			}
		}
		StagingData.resize(DirtyObjectCount * sizeof(FPrimitiveSceneData));

		bool bCreateNewSubResourceContainer = true;
		
		auto TryAddToResoruceUploadBatcher = [&ResourceUpload, &StagingData, &ResoruceUploadBatcher](const int32 ObjectIndex) {
			if (ResourceUpload.SubresourceContainers.size() > 0)
			{
				ResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch =
					StagingData.data() + ObjectIndex * sizeof(FPrimitiveSceneData) - ResourceUpload.SubresourceContainers[0]->SubresourceData.pData;
				ResourceUpload.SubresourceContainers[0]->SubresourceData.SlicePitch = ResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch;

				ResoruceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));
			}
		};

		int32 ObjectIndex = 0;
		for (; ObjectIndex < InDirtyObjectList.size(); ++ObjectIndex)
		{
			if (InDirtyObjectList[ObjectIndex])
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
				PrimitiveSceneData->VisibilityFlags = VisibilityFlags;
				PrimitiveSceneData->LocalToWorld = InRenderObjectList.CachedModelMatrixList[ObjectIndex];
				PrimitiveSceneData->WorldToLocal = InRenderObjectList.CachedModelMatrixList[ObjectIndex].Invert();
			}
			else
			{
				TryAddToResoruceUploadBatcher(ObjectIndex);
				bCreateNewSubResourceContainer = true;
			}
		}
		TryAddToResoruceUploadBatcher(ObjectIndex);

		ResoruceUploadBatcher.Flush(InCommandContext);
		InCommandContext.FlushCommandList(ED3D12QueueType::Direct, true);
	}
}
