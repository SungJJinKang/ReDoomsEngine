#include "D3D12ResourceUploadBatcher.h"

#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "Renderer/Renderer.h"
#include "D3D12CommandList.h"

FD3D12VertexIndexBufferSubresourceContainer::FD3D12VertexIndexBufferSubresourceContainer(const uint8_t* const InData, const size_t InSize, eastl::shared_ptr<Assimp::Importer>& InAssimpImporter)
	: ShadowDataStorage(), Data(InData), Size(InSize), AssimpImporter(InAssimpImporter)
{
	EA_ASSERT_MSG(AssimpImporter, "AssimpImporter is null. To prevent data variable from being dangling pointer, original data should be maintained");
	InitSubresourceData();
}

FD3D12VertexIndexBufferSubresourceContainer::FD3D12VertexIndexBufferSubresourceContainer(eastl::vector<uint8_t>&& InCopiedData)
	: ShadowDataStorage(eastl::move(InCopiedData)), Data(ShadowDataStorage.data()), Size(ShadowDataStorage.size()), AssimpImporter()
{
	InitSubresourceData();
}

void FD3D12VertexIndexBufferSubresourceContainer::InitSubresourceData()
{
	SubresourceData.pData = Data;
	SubresourceData.RowPitch = Size;
	SubresourceData.SlicePitch = SubresourceData.RowPitch;
}

void FD3D12ResourceUploadBatcher::AddPendingResourceUpload(FD3D12ResourceUpload&& InResourceUpload)
{
	EA_ASSERT(GCurrentRendererState == ERendererState::SceneSetup || GCurrentRendererState == ERendererState::OnStartFrame);

	PendingResourceUploadList.emplace_back(eastl::move(InResourceUpload));
}

void FD3D12ResourceUploadBatcher::Flush(FD3D12CommandContext& InCommandContext)
{
	if (PendingResourceUploadList.size() > 0)
	{
		eastl::shared_ptr<FD3D12Fence> UploadBatcherFence = eastl::make_shared<FD3D12Fence>(true);

		eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersBeforeUpload;
		eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersAfterUpload;

		for (FD3D12ResourceUpload& PendingResourceUpload : PendingResourceUploadList)
		{
			ResourceBarriersBeforeUpload.insert(ResourceBarriersBeforeUpload.end(), PendingResourceUpload.ResourceBarriersBeforeUpload.begin(), PendingResourceUpload.ResourceBarriersBeforeUpload.end());
			ResourceBarriersAfterUpload.insert(ResourceBarriersAfterUpload.end(), PendingResourceUpload.ResourceBarriersAfterUpload.begin(), PendingResourceUpload.ResourceBarriersAfterUpload.end());
		}

		FD3D12CommandQueue* const TargetCommandQueue = InCommandContext.CommandQueueList[ED3D12QueueType::Direct]; // @todo : use copy queue

		eastl::shared_ptr<FD3D12CommandList>& CommandListForUploadBatcher = InCommandContext.GraphicsCommandList;
		{
			SCOPED_GPU_TIMER_DIRECT_QUEUE(InCommandContext, FD3D12ResourceUploadBatcher_Flush)

			if (ResourceBarriersBeforeUpload.size() > 0)
			{
				InCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(ResourceBarriersBeforeUpload);
			}

			InCommandContext.GraphicsCommandList->ResourceBarrierBatcher.Flush(*InCommandContext.GraphicsCommandList);

			for (FD3D12ResourceUpload& PendingResourceUpload : PendingResourceUploadList)
			{
				FD3D12UploadBufferContainer* UploadBufferContainer = AllocateUploadBuffer(PendingResourceUpload.Resource);
				const uint32_t SubresourceCount = PendingResourceUpload.SubresourceContainers.size();
				for (uint32_t SubresourceIndex = 0; SubresourceIndex < SubresourceCount; ++SubresourceIndex)
				{
					// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
					// @todo : support mips. change UpdateSubresources's template paramter value
					UpdateSubresources<1>(CommandListForUploadBatcher->GetD3DCommandList(), PendingResourceUpload.Resource, UploadBufferContainer->UploadBuffer->GetResource(),
						0, SubresourceIndex, SubresourceCount, &PendingResourceUpload.SubresourceContainers[SubresourceIndex]->SubresourceData);
				}
				UploadBufferContainer->Fence = InCommandContext.FrameResourceCounter->FrameWorkEndFence;
				UploadBufferContainer->UploadedFrameIndex = GCurrentFrameIndex;
			}

			if (ResourceBarriersAfterUpload.size() > 0)
			{
				InCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(ResourceBarriersAfterUpload);
			}
		}

		PendingResourceUploadList.clear();
	}
}

FD3D12UploadBufferContainer* FD3D12ResourceUploadBatcher::AllocateUploadBuffer(ID3D12Resource* const InUploadedResource)
{
	FD3D12UploadBufferContainer* UploadBufferContainer = nullptr;

	const UINT64 UploadBufferSize = Align(GetRequiredIntermediateSize(InUploadedResource, 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	const ED3D12UploadBufferSizeType BufferSizeType = ConvertSizeToUploadBufferSizeType(UploadBufferSize);
	eastl::queue<eastl::unique_ptr<FD3D12UploadBufferContainer>>& TargetBufferQueue = UploadBufferQueue[static_cast<uint32_t>(BufferSizeType)];

	if (TargetBufferQueue.size() > 0 && TargetBufferQueue.front()->CanFree())
	{
		eastl::unique_ptr<FD3D12UploadBufferContainer> Temp = eastl::move(TargetBufferQueue.front());
		UploadBufferContainer = Temp.get();
		TargetBufferQueue.pop();
		TargetBufferQueue.push(eastl::move(Temp));
	}
	else
	{
		eastl::unique_ptr<FD3D12UploadBufferContainer> NewUploadBufferContainer = eastl::make_unique<FD3D12UploadBufferContainer>();
		NewUploadBufferContainer->UploadBuffer = eastl::make_unique<FD3D12BufferResource>(ConvertUploadBufferSizeTypeToSize(BufferSizeType), D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, 0, true, D3D12_RESOURCE_STATE_GENERIC_READ);
		NewUploadBufferContainer->UploadBuffer->InitResource();
		NewUploadBufferContainer->UploadBuffer->SetDebugNameToResource(EA_WCHAR("Upload Buffer"));
		UploadBufferContainer = NewUploadBufferContainer.get();

		TargetBufferQueue.push(eastl::move(NewUploadBufferContainer));
	}

	EA_ASSERT(UploadBufferContainer);
	return UploadBufferContainer;
}

void FD3D12ResourceUploadBatcher::FreeUnusedUploadBuffers()
{
	for (uint32_t UploadBufferSizeTypeIndex = 0; UploadBufferSizeTypeIndex < static_cast<uint32_t>(ED3D12UploadBufferSizeType::Num); ++UploadBufferSizeTypeIndex)
	{
		eastl::queue<eastl::unique_ptr<FD3D12UploadBufferContainer>>& TargetBufferQueue = UploadBufferQueue[static_cast<uint32_t>(UploadBufferSizeTypeIndex)];
	
		while (TargetBufferQueue.size() > 0 && TargetBufferQueue.front()->CanFree())
		{
			TargetBufferQueue.pop();
		};
	}
}

ED3D12UploadBufferSizeType FD3D12ResourceUploadBatcher::ConvertSizeToUploadBufferSizeType(const uint64_t InSize)
{
	for (uint32_t UploadBufferSizeTypeIndex = 0; UploadBufferSizeTypeIndex < static_cast<uint32_t>(ED3D12UploadBufferSizeType::Num); ++UploadBufferSizeTypeIndex)
	{
		if (InSize <= ConvertUploadBufferSizeTypeToSize(static_cast<ED3D12UploadBufferSizeType>(UploadBufferSizeTypeIndex)))
		{
			return static_cast<ED3D12UploadBufferSizeType>(UploadBufferSizeTypeIndex);
		}
	}

	RD_ASSUME(false);
}

uint64_t FD3D12ResourceUploadBatcher::ConvertUploadBufferSizeTypeToSize(const ED3D12UploadBufferSizeType InUploadBufferSizeType)
{
	switch (InUploadBufferSizeType)
	{
	case ED3D12UploadBufferSizeType::Small:
		return D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	case ED3D12UploadBufferSizeType::Medium:
		return D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 32;
	case ED3D12UploadBufferSizeType::Large:
		return D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 64;
	case ED3D12UploadBufferSizeType::FourK:
		return D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 1024;
	default:
		RD_ASSUME(0);
		break;
	}
}

bool FD3D12UploadBufferContainer::CanFree() const
{
	return (UploadedFrameIndex != GCurrentFrameIndex) && (!(Fence.expired()) ? Fence.lock()->IsCompleteLastSignal() : true);
}

FD3D12ConstantBufferSubresourceContainer::FD3D12ConstantBufferSubresourceContainer(const uint8_t* const InData, const size_t InSize)
	: ShadowDataStorage(), Data(InData), Size(InSize)
{
	InitSubresourceData();
}

FD3D12ConstantBufferSubresourceContainer::FD3D12ConstantBufferSubresourceContainer(const eastl::vector<uint8_t>& InCopiedData)
	: ShadowDataStorage(eastl::move(InCopiedData)), Data(ShadowDataStorage.data()), Size(ShadowDataStorage.size())
{
	InitSubresourceData();
}

FD3D12ConstantBufferSubresourceContainer::FD3D12ConstantBufferSubresourceContainer(eastl::vector<uint8_t>&& InCopiedData)
	: ShadowDataStorage(eastl::move(InCopiedData)), Data(ShadowDataStorage.data()), Size(ShadowDataStorage.size())
{
	InitSubresourceData();
}

void FD3D12ConstantBufferSubresourceContainer::InitSubresourceData()
{
	SubresourceData.pData = Data;
	SubresourceData.RowPitch = Size;
	SubresourceData.SlicePitch = SubresourceData.RowPitch;
}
