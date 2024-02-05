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

	{
		D3D12_HEAP_DESC HeapDesc = {};
		HeapDesc.SizeInBytes = 0;
		HeapDesc.Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HeapDesc.Alignment = 0;
		HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;

		TexturePoolList[FD3D12ResourcePool::EResourcePoolType::RenderTarget] = AllocateNewPool(HeapDesc);
	}


	{
		D3D12_HEAP_DESC HeapDesc = {};
		HeapDesc.SizeInBytes = 0;
		HeapDesc.Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HeapDesc.Alignment = 0;
		HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;

		TexturePoolList[FD3D12ResourcePool::EResourcePoolType::UAVTexture] = AllocateNewPool(HeapDesc);
	}
}

eastl::shared_ptr<FD3D12Texture2DResource> FD3D12ResourceAllocator::Allocate(FD3D12CommandContext& InCommandContext, eastl::vector<FD3D12SubresourceContainer>&& SubresourceDataList,
	const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties, CD3DX12_RESOURCE_DESC InD3DResourceDesc)
{
	EA_ASSERT(InD3DResourceDesc.SampleDesc.Count == 1);
	EA_ASSERT(InResourceCreateProperties.InitialResourceStates == D3D12_RESOURCE_STATE_COPY_DEST);

	eastl::shared_ptr<FD3D12Texture2DResource> D3D12TextureResource{};

	FD3D12ResourcePool::EResourcePoolType TargetTexturePoolType = (InD3DResourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
		? FD3D12ResourcePool::EResourcePoolType::RenderTarget : ((InD3DResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) ? FD3D12ResourcePool::EResourcePoolType::UAVTexture : FD3D12ResourcePool::EResourcePoolType::ReadOnlyTexture);

	D3D12_RESOURCE_ALLOCATION_INFO AllocInfo;
	InD3DResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	AllocInfo = GetD3D12Device()->GetResourceAllocationInfo(0, 1, &InD3DResourceDesc);

	FD3D12ResourcePool& TargetPool = TexturePoolList[TargetTexturePoolType];

	ComPtr<ID3D12Resource> TextureResource;

	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersBeforeUpload{};
	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersAfterUpload{};

	if (TargetPool.HeapDesc.SizeInBytes > 0)
	{
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

		// https://learn.microsoft.com/en-us/windows/win32/direct3d12/upload-and-readback-of-texture-data

		ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr, TextureResource.Get()));
	}
	else
	{
		// create committed resource
		D3D12TextureResource = eastl::make_shared<FD3D12Texture2DResource>(InResourceCreateProperties, InD3DResourceDesc);
	}

	FD3D12ResourceUpload ResourceUpload{};
	ResourceUpload.Resource = D3D12TextureResource;
	ResourceUpload.SubresourceContainer = eastl::move(SubresourceDataList[0]); // @todo support mips
	ResourceUpload.ResourceBarriersBeforeUpload = eastl::move(ResourceBarriersBeforeUpload);
	ResourceUpload.ResourceBarriersAfterUpload = eastl::move(ResourceBarriersAfterUpload);

	ResourceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));

	// Copy initial data to resource memory

	return D3D12TextureResource;
}

void FD3D12ResourceAllocator::OnStartFrame()
{
}

void FD3D12ResourceAllocator::OnEndFrame()
{
}

FD3D12ResourcePool FD3D12ResourceAllocator::AllocateNewPool(const D3D12_HEAP_DESC InHeapDesc)
{
	FD3D12ResourcePool Pool{};

	Pool.HeapDesc = InHeapDesc;
	if (Pool.HeapDesc.SizeInBytes > 0)
	{
		eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& NewResourcePoolHeapContainer = Pool.ResourcePoolHeapContainerList.emplace_back(eastl::make_shared<FD3D12ResourcePoolHeapContainer>());
		VERIFYD3D12RESULT(GetD3D12Device()->CreateHeap(&(Pool.HeapDesc), IID_PPV_ARGS(&(NewResourcePoolHeapContainer->Heap))));

		FD3D12ResourcePoolBlock FreeBlock{};
		FreeBlock.Size = Pool.HeapDesc.SizeInBytes;
		FreeBlock.OffsetFromBase = 0;

		NewResourcePoolHeapContainer->FreeBlockList.emplace_back(FreeBlock);
	}

	return Pool;
}
