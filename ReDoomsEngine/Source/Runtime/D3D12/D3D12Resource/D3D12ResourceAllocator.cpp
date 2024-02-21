#include "D3D12ResourceAllocator.h"

#include "D3D12Device.h"
#include "D3D12Resource.h"

static bool IsFormatCompressed(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return true;
    default:
        return false;
    }
}

// Only some formats are supported. For others it returns 0.
static UINT GetBitsPerPixel(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return 64;
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        return 64;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return 32;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return 32;
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        return 32;
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        return 32;
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return 32;
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        return 16;
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        return 16;
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return 4;
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return 8;
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return 8;
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return 8;
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        return 8;
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;
    default:
        return 0;
    }
}

// copy from https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator/commit/c7f78c8c083735ed2e34c277cd35e1e61556461b
// This algorithm is overly conservative.
static bool CanUseSmallAlignment(const D3D12_RESOURCE_DESC& resourceDesc)
{
    if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        return false;
    if ((resourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) != 0)
        return false;
    if (resourceDesc.SampleDesc.Count > 1)
        return false;
    if (resourceDesc.DepthOrArraySize != 1)
        return false;

    UINT sizeX = (UINT)resourceDesc.Width;
    UINT sizeY = resourceDesc.Height;
    UINT bitsPerPixel = GetBitsPerPixel(resourceDesc.Format);
    if (GetBitsPerPixel == 0)
        return false;

    if (IsFormatCompressed(resourceDesc.Format))
    {
        sizeX = DivideRoudingUp(sizeX / 4, 1u);
        sizeY = DivideRoudingUp(sizeY / 4, 1u);
        bitsPerPixel *= 16;
    }

    UINT tileSizeX = 0, tileSizeY = 0;
    switch (bitsPerPixel)
    {
    case   8: tileSizeX = 64; tileSizeY = 64; break;
    case  16: tileSizeX = 64; tileSizeY = 32; break;
    case  32: tileSizeX = 32; tileSizeY = 32; break;
    case  64: tileSizeX = 32; tileSizeY = 16; break;
    case 128: tileSizeX = 16; tileSizeY = 16; break;
    default: return false;
    }

    const UINT tileCount = DivideRoudingUp(sizeX, tileSizeX) * DivideRoudingUp(sizeY, tileSizeY);
    return tileCount <= 16;
}

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
	EA_ASSERT(InD3DResourceDesc.SampleDesc.Count == 1); // doesn't support msaa for now
	EA_ASSERT(InResourceCreateProperties.InitialResourceStates == D3D12_RESOURCE_STATE_COPY_DEST);

	eastl::shared_ptr<FD3D12Texture2DResource> D3D12TextureResource{};

	FD3D12ResourceUpload ResourceUpload{};

	if (!(InD3DResourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)))
	{
		// https://learn.microsoft.com/en-us/windows/win32/direct3d12/upload-and-readback-of-texture-data


		FD3D12ResourcePool::EResourcePoolType TargetTexturePoolType = FD3D12ResourcePool::EResourcePoolType::ReadOnlyTexture;

		D3D12_RESOURCE_ALLOCATION_INFO AllocInfo;

		if (CanUseSmallAlignment(InD3DResourceDesc))
		{
            InD3DResourceDesc.Alignment = D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT;
		}
        else
        {
            InD3DResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        }
		AllocInfo = GetD3D12Device()->GetResourceAllocationInfo(0, 1, &InD3DResourceDesc);

		// Below code doesn't work for now.. Debug layer complains about invalid alignment
		// https://www.asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment
// 		InD3DResourceDesc.Alignment = D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT;
// 		AllocInfo = GetD3D12Device()->GetResourceAllocationInfo(0, 1, &InD3DResourceDesc);
// 
// 		if (AllocInfo.Alignment != D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT)
// 		{
// 			// If the alignment requested is not granted, then let D3D tell us
// 			// the alignment that needs to be used for these resources.
// 			InD3DResourceDesc.Alignment = 0;
// 			AllocInfo = GetD3D12Device()->GetResourceAllocationInfo(0, 1, &InD3DResourceDesc);
// 		}

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

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticVertexBuffer(FD3D12CommandContext& InCommandContext, const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes, eastl::shared_ptr<Assimp::Importer> Importer)
{
	eastl::unique_ptr<FD3D12VertexIndexBufferSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(Data, InSize);
    SubresourceContainer->Importer = Importer;
	return AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), InDefaultStrideInBytes);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticIndexBuffer(FD3D12CommandContext& InCommandContext, eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes)
{
	return AllocateStaticVertexIndexBuffer(InCommandContext, eastl::move(SubresourceDataList), InDefaultStrideInBytes, false);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticIndexBuffer(FD3D12CommandContext& InCommandContext, const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes, eastl::shared_ptr<Assimp::Importer> Importer)
{
	eastl::unique_ptr<FD3D12VertexIndexBufferSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(Data, InSize);
    SubresourceContainer->Importer = Importer;
	return AllocateStaticIndexBuffer(InCommandContext, eastl::move(SubresourceContainer), InDefaultStrideInBytes);
}

eastl::shared_ptr<FD3D12VertexIndexBufferResource> FD3D12ResourceAllocator::AllocateStaticVertexIndexBuffer(FD3D12CommandContext& InCommandContext,
	eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes, const bool bVertexBuffer)
{
    // @todo implment vertex/index buffer pool. currently a lot of unused memory is wasted because of d3d12 64kb alignment limitation.
    // Create large heap and sub-allocate vertex/index buffer resource on it
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
