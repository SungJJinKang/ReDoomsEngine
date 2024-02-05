#include "D3D12ResourceUploadBatcher.h"

#include "D3D12CommandList.h"
#include "D3D12Device.h"

void FD3D12ResourceUploadBatcher::AddPendingResourceUpload(const FResourceUpload& InResourceUpload)
{
	PendingResourceUploadList.emplace_back(InResourceUpload);
}

void FD3D12ResourceUploadBatcher::Flush(FD3D12CommandContext& InCommandContext)
{
	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersBeforeUpload;
	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersAfterUpload;

	for (FResourceUpload& PendingResourceUpload : PendingResourceUploadList)
	{
		ResourceBarriersBeforeUpload.insert(ResourceBarriersBeforeUpload.end(), PendingResourceUpload.ResourceBarriersBeforeUpload.begin(), PendingResourceUpload.ResourceBarriersBeforeUpload.end());
		ResourceBarriersAfterUpload.insert(ResourceBarriersAfterUpload.end(), PendingResourceUpload.ResourceBarriersAfterUpload.begin(), PendingResourceUpload.ResourceBarriersAfterUpload.end());
	}

	if (ResourceBarriersBeforeUpload.size() > 0)
	{
		InCommandContext.CommandList->GetD3DCommandList()->ResourceBarrier(ResourceBarriersBeforeUpload.size(), ResourceBarriersBeforeUpload.data());
	}

	for (FResourceUpload& PendingResourceUpload : PendingResourceUploadList)
	{
		FD3D12BufferResource* const UploadBuffer = AllocateUploadBuffer(PendingResourceUpload.Resource.get());
		UpdateSubresources<1>(InCommandContext.CommandList->GetD3DCommandList(), PendingResourceUpload.Resource->GetResource(), UploadBuffer->GetResource(), 0, 0, 1, &PendingResourceUpload.SubresourceData);
		UploadBuffer->Fence.Signal(FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct));
	}

	if (ResourceBarriersAfterUpload.size() > 0)
	{
		InCommandContext.CommandList->GetD3DCommandList()->ResourceBarrier(ResourceBarriersAfterUpload.size(), ResourceBarriersAfterUpload.data());
	}

}

FD3D12BufferResource* FD3D12ResourceUploadBatcher::AllocateUploadBuffer(const FD3D12Resource* const InResource)
{
	FD3D12BufferResource* Buffer = nullptr;

	const UINT64 UploadBufferSize = Align(GetRequiredIntermediateSize(InResource->GetResource(), 0, 1), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	const EUploadBufferSizeType BufferSizeType = ConvertSizeToUploadBufferSizeType(UploadBufferSize);
	eastl::queue<eastl::unique_ptr<FD3D12BufferResource>>& TargetQueue = UploadBufferQueue[BufferSizeType];

	if (TargetQueue.size() > 0 && TargetQueue.front()->Fence.IsCompleteLastSignal())
	{
		TargetQueue.push(eastl::move(TargetQueue.front()));
		Buffer = TargetQueue.back().get();
		TargetQueue.pop();
	}
	else
	{
		TargetQueue.push(eastl::make_unique<FD3D12BufferResource>(ConvertUploadBufferSizeTypeToSize(BufferSizeType), D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE, 0, true));
		Buffer = TargetQueue.back().get();
		Buffer->InitResource();
		Buffer->Fence.Init();
	}

	EA_ASSERT(Buffer);
	return Buffer;
}

EUploadBufferSizeType FD3D12ResourceUploadBatcher::ConvertSizeToUploadBufferSizeType(const uint64_t InSize)
{
	return EUploadBufferSizeType::VeryLarge;
}

uint64_t FD3D12ResourceUploadBatcher::ConvertUploadBufferSizeTypeToSize(const EUploadBufferSizeType InUploadBufferSizeType)
{
	switch (InUploadBufferSizeType)
	{
	case Small:
	case Medium:
	case Large:
	case VeryLarge:
		return 1024 * 1024 * 16; // @todo elborate this
	default:
		EA_ASSUME(0);
		break;
	}
}
