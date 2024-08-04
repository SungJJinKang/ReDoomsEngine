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

	PrimitiveIDBuffer = eastl::make_shared<FD3D12VertexIndexBufferResource>(GSupportedMaxPrimitiveCount * sizeof(uint32), sizeof(uint32), true);
	PrimitiveIDBuffer->InitResource();
	PrimitiveIDBuffer->SetDebugNameToResource(EA_WCHAR("PrimitiveIDBuffer"));
}

void GPUScene::UploadDirtyData(FD3D12CommandContext& InCommandContext, FRenderObjectList& InRenderObjectList)
{
	if (!InRenderObjectList.GPUSceneDirtyObjectList.empty())
	{
		bool bAnyUpload = false;
		EA_ASSERT_MSG(InRenderObjectList.GPUSceneDirtyObjectList.size() <= GSupportedMaxPrimitiveCount, "Exceed max primitive count, Please increase it.");
		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(InCommandContext, GPUScene_UploadDirtyData);

			FD3D12ResourceUploadBatcher& ResoruceUploadBatcher =
				EA::StdC::Singleton<FD3D12ResourceAllocator>::GetInstance()->ResourceUploadBatcher;

			FD3D12ResourceUpload GPUSceneBufferResourceUpload{};
			FD3D12ResourceUpload PrimitiveIDBufferResourceUpload{};
			auto SetupResourceUpload = [&GPUSceneBufferResourceUpload, &PrimitiveIDBufferResourceUpload, this]() {
				GPUSceneBufferResourceUpload.Resource = GPUSceneBuffer->GetResource();
				GPUSceneBufferResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GPUSceneBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));
				GPUSceneBufferResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GPUSceneBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

				PrimitiveIDBufferResourceUpload.Resource = PrimitiveIDBuffer->GetResource();
				PrimitiveIDBufferResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(PrimitiveIDBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));
				PrimitiveIDBufferResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(PrimitiveIDBufferResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
				};
			SetupResourceUpload();

			eastl::vector<uint8> GPUSceneBufferStagingData{};
			eastl::vector<uint8> PrimitiveIDBufferStagingData{};

			uint32 DirtyObjectCount = 0;
			// @TODO : Implement count function like bitset's
			for (int32 ObjectIndex = 0; ObjectIndex < InRenderObjectList.GPUSceneDirtyObjectList.size(); ++ObjectIndex)
			{
				if (InRenderObjectList.GPUSceneDirtyObjectList[ObjectIndex])
				{
					++DirtyObjectCount;
				}
			}

			const uint64 RequiredGPUSceneBufferStagingBufferSize = DirtyObjectCount * sizeof(FPrimitiveSceneData);
			const uint64 RequiredPrimitiveIDBufferStagingBufferSize = DirtyObjectCount * sizeof(uint32);

			const uint64 MinGPUSceneBufferStagingBufferSize = Align(GetRequiredIntermediateSize(GPUSceneBuffer->GetResource(), 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
			const uint64 MinPrimitiveIDBufferStagingBufferSize = Align(GetRequiredIntermediateSize(PrimitiveIDBuffer->GetResource(), 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

			GPUSceneBufferStagingData.resize(eastl::max(RequiredGPUSceneBufferStagingBufferSize, MinGPUSceneBufferStagingBufferSize));
			PrimitiveIDBufferStagingData.resize(eastl::max(RequiredPrimitiveIDBufferStagingBufferSize, MinPrimitiveIDBufferStagingBufferSize));
			
			bool bCreateNewSubResourceContainer = true;

			auto TryAddToResoruceUploadBatcher = [&bAnyUpload, &GPUSceneBufferResourceUpload, &PrimitiveIDBufferResourceUpload, &GPUSceneBufferStagingData, &PrimitiveIDBufferStagingData , &ResoruceUploadBatcher, &SetupResourceUpload](const int32 ObjectIndex) {
				if (GPUSceneBufferResourceUpload.SubresourceContainers.size() > 0)
				{
					GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch =
						GPUSceneBufferStagingData.data() + ObjectIndex * sizeof(FPrimitiveSceneData) - reinterpret_cast<const uint8*>(GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData);
					GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.SlicePitch = GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch;

					PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch =
						PrimitiveIDBufferStagingData.data() + ObjectIndex * sizeof(uint32) - reinterpret_cast<const uint8*>(PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData);
					EA_ASSERT(
						reinterpret_cast<const uint8*>(PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData) + PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch
						<=
						PrimitiveIDBufferStagingData.end());
					PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.SlicePitch = PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.RowPitch;

					ResoruceUploadBatcher.AddPendingResourceUpload(eastl::move(GPUSceneBufferResourceUpload));
					ResoruceUploadBatcher.AddPendingResourceUpload(eastl::move(PrimitiveIDBufferResourceUpload));
					SetupResourceUpload();

					bAnyUpload = true;
				}
			};

			int32 ObjectIndex = 0;
			for (; ObjectIndex < InRenderObjectList.GPUSceneDirtyObjectList.size(); ++ObjectIndex)
			{
				if (InRenderObjectList.GPUSceneDirtyObjectList[ObjectIndex])
				{
					EA_ASSERT(RequiredGPUSceneBufferStagingBufferSize > ObjectIndex * sizeof(FPrimitiveSceneData));
					EA_ASSERT(RequiredPrimitiveIDBufferStagingBufferSize > ObjectIndex * sizeof(uint32));

					FPrimitiveSceneData* PrimitiveSceneData = reinterpret_cast<FPrimitiveSceneData*>(GPUSceneBufferStagingData.data() + ObjectIndex * sizeof(FPrimitiveSceneData));
					uint32* PrimitiveID = reinterpret_cast<uint32*>(PrimitiveIDBufferStagingData.data() + ObjectIndex * sizeof(uint32));

					if (bCreateNewSubResourceContainer)
					{
						GPUSceneBufferResourceUpload.SubresourceContainers.emplace_back(eastl::make_unique<FD3D12SubresourceContainer>());
						GPUSceneBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData = PrimitiveSceneData;
						GPUSceneBufferResourceUpload.SubresourceContainers[0]->DstOffset = ObjectIndex * sizeof(FPrimitiveSceneData);

						PrimitiveIDBufferResourceUpload.SubresourceContainers.emplace_back(eastl::make_unique<FD3D12SubresourceContainer>());
						PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->SubresourceData.pData = PrimitiveID;
						PrimitiveIDBufferResourceUpload.SubresourceContainers[0]->DstOffset = ObjectIndex * sizeof(uint32);
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

					*PrimitiveID = ObjectIndex;

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

		if (bAnyUpload)
		{
			InCommandContext.FlushCommandList(ED3D12QueueType::Direct, true);
			InCommandContext.ResetCommandList(ED3D12QueueType::Direct);
			InCommandContext.StateCache.ResetForNewCommandlist();

			PrimitiveCount = InRenderObjectList.GPUSceneDirtyObjectList.size();
		}
	}
}

FD3D12ConstantBufferResource* GPUScene::GetGPUSceneBuffer()
{
	return GPUSceneBuffer.get();
}

FD3D12VertexIndexBufferResource* GPUScene::GetPrimitiveIDBuffer()
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
