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

	PrimitiveIDBuffer = eastl::make_shared<FD3D12ConstantBufferResource>(GSupportedMaxPrimitiveCount * sizeof(uint32), true);
	PrimitiveIDBuffer->InitResource();;
}

void GPUScene::UploadDirtyData(FD3D12CommandContext& InCommandContext, FPrimitiveList& InPrimitiveList)
{
	if (!InPrimitiveList.GPUSceneDirtyPrimitiveList.empty())
	{
		bool bAnyUpload = false;
		EA_ASSERT_MSG(InPrimitiveList.GPUSceneDirtyPrimitiveList.size() <= GSupportedMaxPrimitiveCount, "Exceed max primitive count, Please increase it.");
		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(InCommandContext, GPUScene_UploadDirtyData);

			FD3D12ResourceUploadBatcher& ResoruceUploadBatcher =
				EA::StdC::Singleton<FD3D12ResourceAllocator>::GetInstance()->ResourceUploadBatcher;

			FD3D12ResourceUpload GPUSceneBufferResourceUpload{};
			auto SetupResourceUpload = [&GPUSceneBufferResourceUpload, this]() {
				GPUSceneBufferResourceUpload.Resource = GPUSceneBuffer->GetResource();
				GPUSceneBufferResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GPUSceneBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));
				GPUSceneBufferResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GPUSceneBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
				};
			SetupResourceUpload();

			eastl::vector<uint8> GPUSceneBufferStagingData{};

			uint32 DirtyPrimitiveCount = 0;
			// @TODO : Implement count function like bitset's
			for (int32 PrimitiveIndex = 0; PrimitiveIndex < InPrimitiveList.GPUSceneDirtyPrimitiveList.size(); ++PrimitiveIndex)
			{
				if (InPrimitiveList.GPUSceneDirtyPrimitiveList[PrimitiveIndex])
				{
					++DirtyPrimitiveCount;
				}
			}

			const uint64 RequiredGPUSceneBufferStagingBufferSize = DirtyPrimitiveCount * sizeof(FPrimitiveSceneData);

			const uint64 MinGPUSceneBufferStagingBufferSize = Align(GetRequiredIntermediateSize(GPUSceneBuffer->GetResource(), 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		
			GPUSceneBufferStagingData.resize(eastl::max(RequiredGPUSceneBufferStagingBufferSize, MinGPUSceneBufferStagingBufferSize));
			
			bool bCreateNewSubResourceContainer = true;

			auto TryAddToResoruceUploadBatcher = [&bAnyUpload, &GPUSceneBufferResourceUpload, &GPUSceneBufferStagingData , &ResoruceUploadBatcher, &SetupResourceUpload](const int32 PrimitiveIndex) {
				if (GPUSceneBufferResourceUpload.SubresourceContainers.size() > 0)
				{
					GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch =
						GPUSceneBufferStagingData.data() + PrimitiveIndex * sizeof(FPrimitiveSceneData) - reinterpret_cast<const uint8*>(GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData);
					GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.SlicePitch = GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch;

					ResoruceUploadBatcher.AddPendingResourceUpload(eastl::move(GPUSceneBufferResourceUpload));
					SetupResourceUpload();

					bAnyUpload = true;
				}
			};

			int32 PrimitiveIndex = 0;
			for (; PrimitiveIndex < InPrimitiveList.GPUSceneDirtyPrimitiveList.size(); ++PrimitiveIndex)
			{
				if (InPrimitiveList.GPUSceneDirtyPrimitiveList[PrimitiveIndex])
				{

					FPrimitiveSceneData* PrimitiveSceneData = reinterpret_cast<FPrimitiveSceneData*>(GPUSceneBufferStagingData.data() + PrimitiveIndex * sizeof(FPrimitiveSceneData));

					if (bCreateNewSubResourceContainer)
					{
						GPUSceneBufferResourceUpload.SubresourceContainers.emplace_back(eastl::make_unique<FD3D12SubresourceContainer>());
						GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData = PrimitiveSceneData;
						GPUSceneBufferResourceUpload.SubresourceContainers[0]->DstOffset = PrimitiveIndex * sizeof(FPrimitiveSceneData);

						bCreateNewSubResourceContainer = false;
					}

					// Fill data

					uint32 VisibilityFlags = 0;
					for (uint32 PassIndex = 0; PassIndex < static_cast<uint32>(EPass::Num); ++PassIndex)
					{
						if (InPrimitiveList.VisibleFlagsList[PassIndex][PrimitiveIndex])
						{
							VisibilityFlags |= (1 << PassIndex);
						}
					}
					PrimitiveSceneData->Flags = InPrimitiveList.PrimitiveFlagList[PrimitiveIndex];
					PrimitiveSceneData->VisibilityFlags = VisibilityFlags;
					PrimitiveSceneData->LocalToWorld = InPrimitiveList.CachedLocalToWorldMatrixList[PrimitiveIndex];
					PrimitiveSceneData->WorldToLocal = InPrimitiveList.CachedLocalToWorldMatrixList[PrimitiveIndex].Invert();

					InPrimitiveList.GPUSceneDirtyPrimitiveList[PrimitiveIndex] = false;
				}
				else
				{
					TryAddToResoruceUploadBatcher(PrimitiveIndex);
					bCreateNewSubResourceContainer = true;
				}
			}
			TryAddToResoruceUploadBatcher(PrimitiveIndex);

			ResoruceUploadBatcher.Flush(InCommandContext);
		}

		if (bAnyUpload)
		{
			InCommandContext.FlushCommandList(ED3D12QueueType::Direct, true);
			InCommandContext.ResetCommandList(ED3D12QueueType::Direct);
			InCommandContext.StateCache.ResetForNewCommandlist();

			PrimitiveCount = InPrimitiveList.GPUSceneDirtyPrimitiveList.size();
		}
	}
}

FD3D12ConstantBufferResource* GPUScene::GetGPUSceneBuffer()
{
	return GPUSceneBuffer.get();
}

FD3D12ConstantBufferResource* GPUScene::GetPrimitiveIDBuffer()
{
	return PrimitiveIDBuffer.get();
}

uint32 GPUScene::GetPrimitiveCount() const
{
	return PrimitiveCount;
}

uint32 GPUScene::GetSceneBufferSize() const
{
	return GPUSceneBuffer->GetBufferSize();
}
