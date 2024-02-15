#include "D3D12ResourceAllocator.h"

#include "D3D12Device.h"
#include "D3D12Resource.h"

void FD3D12ResourceAllocator::Init()
{
	{
		D3D12_HEAP_DESC HeapDesc = {};
		HeapDesc.SizeInBytes = 1024 * 1024 * 64;
		HeapDesc.Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HeapDesc.Alignment = 0;
		HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;

		TexturePoolList[FD3D12ResourcePool::EResourcePoolType::ReadOnlyTexture] = AllocateNewPool(HeapDesc);
	}
}

eastl::shared_ptr<FD3D12Texture2DResource> FD3D12ResourceAllocator::AllocateTexture2D(FD3D12CommandContext& InCommandContext, eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>>&& SubresourceDataList,
	const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties, CD3DX12_RESOURCE_DESC InD3DResourceDesc, const eastl::optional<D3D12_RESOURCE_STATES>& InResourceStateAfterUpload)
{
	EA_ASSERT(InD3DResourceDesc.SampleDesc.Count == 1);
	EA_ASSERT(InResourceCreateProperties.InitialResourceStates == D3D12_RESOURCE_STATE_COPY_DEST);

	eastl::shared_ptr<FD3D12Texture2DResource> D3D12TextureResource{};

	FD3D12ResourceUpload ResourceUpload{};

	if (!(InD3DResourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)))
	{
		// https://learn.microsoft.com/en-us/windows/win32/direct3d12/upload-and-readback-of-texture-data


		FD3D12ResourcePool::EResourcePoolType TargetTexturePoolType = FD3D12ResourcePool::EResourcePoolType::ReadOnlyTexture;

		D3D12_RESOURCE_ALLOCATION_INFO AllocInfo;
		InD3DResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		AllocInfo = GetD3D12Device()->GetResourceAllocationInfo(0, 1, &InD3DResourceDesc);

		ComPtr<ID3D12Resource> TextureResource;

		FD3D12ResourcePool& TargetPool = TexturePoolList[TargetTexturePoolType];
		FD3D12ResourcePoolBlock OutAllocatedBlock{};
		const bool bIsSuccess = TargetPool.AllocateBlock(AllocInfo.SizeInBytes, AllocInfo.Alignment, OutAllocatedBlock);
		EA_ASSERT(bIsSuccess);

		InD3DResourceDesc.Alignment = TargetPool.HeapDesc.Alignment; // Follow heap's alignment

		VERIFYD3D12RESULT(GetD3D12Device()->CreatePlacedResource(
			OutAllocatedBlock.OwnerResourcePoolHeapContainer.lock()->Heap.Get(),
			OutAllocatedBlock.OffsetFromBase, // Offset in bytes from the beginning of the heap
			&InD3DResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&TextureResource)
		));

		D3D12TextureResource = eastl::make_shared<FD3D12Texture2DResource>(TextureResource, OutAllocatedBlock, InResourceCreateProperties, InD3DResourceDesc);
		D3D12TextureResource->InitResource();

		// Why providing pResourceBefore is faster? : https://www.gamedev.net/forums/topic/691943-resource-aliasing-barriers/5357916/

		// Even if doesn't issue aliasing barrier, it works well and deubg layter doesn't complain
		ResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr, TextureResource.Get()));
	}
	else
	{
		// create committed resource
		D3D12TextureResource = eastl::make_shared<FD3D12Texture2DResource>(InResourceCreateProperties, InD3DResourceDesc);
	}

	ResourceUpload.Resource = D3D12TextureResource;
	EA_ASSERT(SubresourceDataList.size() == 1); // @todo : support mips
	ResourceUpload.SubresourceContainers = eastl::move(SubresourceDataList);
	if (InResourceStateAfterUpload.has_value())
	{
		ResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(D3D12TextureResource->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, *InResourceStateAfterUpload));
	}

	ResourceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));

	// Copy initial data to resource memory

	return D3D12TextureResource;
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticVertexBuffer(FD3D12CommandContext& InCommandContext, eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes)
{
	return AllocateStaticVertexIndexBuffer(InCommandContext, eastl::move(SubresourceDataList), InDefaultStrideInBytes, true);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticVertexBuffer(FD3D12CommandContext& InCommandContext, const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes)
{
	eastl::unique_ptr<FD3D12VertexIndexBufferSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(Data, InSize);
	return AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), InDefaultStrideInBytes);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticIndexBuffer(FD3D12CommandContext& InCommandContext, eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes)
{
	return AllocateStaticVertexIndexBuffer(InCommandContext, eastl::move(SubresourceDataList), InDefaultStrideInBytes, false);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticIndexBuffer(FD3D12CommandContext& InCommandContext, const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes)
{
	eastl::unique_ptr<FD3D12VertexIndexBufferSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(Data, InSize);
	return AllocateStaticIndexBuffer(InCommandContext, eastl::move(SubresourceContainer), InDefaultStrideInBytes);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticVertexIndexBuffer(FD3D12CommandContext& InCommandContext,
	eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes, const bool bVertexBuffer)
{
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> VertexIndexBufferResource =  eastl::make_shared<FD3D12VertexIndexBufferResource>(SubresourceDataList->SubresourceData.RowPitch, InDefaultStrideInBytes, false);
	VertexIndexBufferResource->InitResource();

	FD3D12ResourceUpload ResourceUpload{};
	ResourceUpload.Resource = VertexIndexBufferResource;
	ResourceUpload.SubresourceContainers.emplace_back(eastl::move(SubresourceDataList));
	ResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(VertexIndexBufferResource->GetResource(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
		bVertexBuffer ? D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER : D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER));

	ResourceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));

	// Copy initial data to resource memory

	return VertexIndexBufferResource;
}

void FD3D12ResourceAllocator::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
}

void FD3D12ResourceAllocator::OnEndFrame(FD3D12CommandContext& InCommandContext)
{
}

FD3D12ResourcePool FD3D12ResourceAllocator::AllocateNewPool(const D3D12_HEAP_DESC InHeapDesc)
{
	FD3D12ResourcePool Pool{};

	EA_ASSERT(InHeapDesc.SizeInBytes > 0);
	eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& NewResourcePoolHeapContainer = Pool.ResourcePoolHeapContainerList.emplace_back(eastl::make_shared<FD3D12ResourcePoolHeapContainer>());
	VERIFYD3D12RESULT(GetD3D12Device()->CreateHeap(&(InHeapDesc), IID_PPV_ARGS(&(NewResourcePoolHeapContainer->Heap))));

	FD3D12ResourcePoolBlock FreeBlock{};
	FreeBlock.Size = InHeapDesc.SizeInBytes;
	FreeBlock.OffsetFromBase = 0;

	NewResourcePoolHeapContainer->FreeBlockList.emplace_back(FreeBlock);
	
	return Pool;
}
