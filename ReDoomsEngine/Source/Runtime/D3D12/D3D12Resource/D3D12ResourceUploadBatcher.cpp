#include "D3D12ResourceUploadBatcher.h"

#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "Renderer.h"
#include "D3D12CommandList.h"

FD3D12VertexIndexBufferSubresourceContainer::FD3D12VertexIndexBufferSubresourceContainer(const uint8_t* const Data, const size_t InSize)
	: VertexIndexData()
{
	VertexIndexData.resize(InSize);
	EA::StdC::Memcpy(VertexIndexData.data(), Data, InSize);

	SubresourceData.pData = VertexIndexData.data();
	SubresourceData.RowPitch = InSize;
	SubresourceData.SlicePitch = SubresourceData.RowPitch;
}

FD3D12VertexIndexBufferSubresourceContainer::FD3D12VertexIndexBufferSubresourceContainer(eastl::vector<uint8_t>&& InVertexIndexData)
	: VertexIndexData(eastl::move(InVertexIndexData))
{
	SubresourceData.pData = VertexIndexData.data();
	SubresourceData.RowPitch = VertexIndexData.size();
	SubresourceData.SlicePitch = SubresourceData.RowPitch;
}

void FD3D12ResourceUploadBatcher::AddPendingResourceUpload(FD3D12ResourceUpload&& InResourceUpload)
{
	EA_ASSERT(FRenderer::GetInstance()->GetCurrentRendererState() == ERendererState::OnStartFrame);

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
				CommandListForUploadBatcher->GetD3DCommandList()->ResourceBarrier(ResourceBarriersBeforeUpload.size(), ResourceBarriersBeforeUpload.data());
			}

			for (FD3D12ResourceUpload& PendingResourceUpload : PendingResourceUploadList)
			{
				FD3D12UploadBufferContainer* UploadBufferContainer = AllocateUploadBuffer(PendingResourceUpload.Resource.get());
				const uint32_t SubresourceCount = PendingResourceUpload.SubresourceContainers.size();
				for (uint32_t SubresourceIndex = 0; SubresourceIndex < SubresourceCount; ++SubresourceIndex)
				{
					// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
					// @todo : support mips. change UpdateSubresources's template paramter value
					UpdateSubresources<1>(CommandListForUploadBatcher->GetD3DCommandList(), PendingResourceUpload.Resource->GetResource(), UploadBufferContainer->UploadBuffer->GetResource(),
						0, SubresourceIndex, SubresourceCount, &PendingResourceUpload.SubresourceContainers[SubresourceIndex]->SubresourceData);
				}
				UploadBufferContainer->Fence = InCommandContext.FrameResourceCounter->FrameWorkEndFence;
			}

			if (ResourceBarriersAfterUpload.size() > 0)
			{
				CommandListForUploadBatcher->GetD3DCommandList()->ResourceBarrier(ResourceBarriersAfterUpload.size(), ResourceBarriersAfterUpload.data());
			}
		}

		PendingResourceUploadList.clear();
	}
}

FD3D12UploadBufferContainer* FD3D12ResourceUploadBatcher::AllocateUploadBuffer(const FD3D12Resource* const InUploadedResource)
{
	FD3D12UploadBufferContainer* UploadBufferContainer = nullptr;

	const UINT64 UploadBufferSize = Align(GetRequiredIntermediateSize(InUploadedResource->GetResource(), 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	const ED3D12UploadBufferSizeType BufferSizeType = ConvertSizeToUploadBufferSizeType(UploadBufferSize);
	eastl::queue<eastl::unique_ptr<FD3D12UploadBufferContainer>>& TargetBufferQueue = UploadBufferQueue[static_cast<uint32_t>(BufferSizeType)];

	if (TargetBufferQueue.size() > 0 && (!(TargetBufferQueue.front()->Fence.expired()) ? TargetBufferQueue.front()->Fence.lock()->IsCompleteLastSignal() : true))
	{
		eastl::unique_ptr<FD3D12UploadBufferContainer> Temp = eastl::move(TargetBufferQueue.front());
		UploadBufferContainer = Temp.get();
		TargetBufferQueue.pop();
		TargetBufferQueue.push(eastl::move(Temp));
	}
	else
	{
		eastl::unique_ptr<FD3D12UploadBufferContainer> NewUploadBufferContainer = eastl::make_unique<FD3D12UploadBufferContainer>();
		NewUploadBufferContainer->UploadBuffer = eastl::make_unique<FD3D12BufferResource>(ConvertUploadBufferSizeTypeToSize(BufferSizeType), D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, 0, true);
		NewUploadBufferContainer->UploadBuffer->InitResource();
		NewUploadBufferContainer->UploadBuffer->SetDebugNameToResource(EA_WCHAR("Upload Buffer"));
		UploadBufferContainer = NewUploadBufferContainer.get();

		TargetBufferQueue.push(eastl::move(NewUploadBufferContainer));
	}

	EA_ASSERT(UploadBufferContainer);
	return UploadBufferContainer;
}

ED3D12UploadBufferSizeType FD3D12ResourceUploadBatcher::ConvertSizeToUploadBufferSizeType(const uint64_t InSize)
{
	return ED3D12UploadBufferSizeType::VeryLarge;
}

uint64_t FD3D12ResourceUploadBatcher::ConvertUploadBufferSizeTypeToSize(const ED3D12UploadBufferSizeType InUploadBufferSizeType)
{
	switch (InUploadBufferSizeType)
	{
	case ED3D12UploadBufferSizeType::Small:
	case ED3D12UploadBufferSizeType::Medium:
	case ED3D12UploadBufferSizeType::Large:
	case ED3D12UploadBufferSizeType::VeryLarge:
		return 1024 * 1024 * 16; // @todo elborate this
	default:
		RD_ASSUME(0);
		break;
	}
}
