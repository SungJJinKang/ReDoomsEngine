#include "D3D12Resource.h"

#include "D3D12Device.h"
#include "D3D12ConstantBufferRingBuffer.h"
#include "Renderer/Renderer.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"

FD3D12Resource::FD3D12Resource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: Fence(), ResourceCreateProperties(InResourceCreateProperties), Desc(InDesc), bInit(false), Resources(),
	DefaultCBV(), DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
	ValidateResourceProperties();
}

FD3D12Resource::FD3D12Resource(ComPtr<ID3D12Resource>& InResource)
	: Fence(), ResourceCreateProperties(), bInit(false),
	DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
	Resources[0] = InResource;
	Desc = CD3DX12_RESOURCE_DESC{ Resources[0]->GetDesc() };

	Resources[0]->GetHeapProperties(&ResourceCreateProperties.HeapProperties, &ResourceCreateProperties.HeapFlags);

	ValidateResourceProperties();
}

FD3D12Resource::FD3D12Resource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: Fence(), ResourceCreateProperties(InResourceCreateProperties), Desc(InDesc), bInit(false),
	DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
	Resources[0] = InResource;
}

void FD3D12Resource::InitResource()
{
	bInit = true;

	if (IsCreateD3D12ResourceOnInitResource() && !Resources[0])
	{
		CreateD3D12Resource();
	}
}

void FD3D12Resource::DeferredRelease()
{
	FRenderer::GetInstance()->GetCurrentFrameResourceContainer().DeferredDeletedResourceList.push_back(weak_from_this());
}

void FD3D12Resource::CreateD3D12Resource()
{
	EA_ASSERT(bInit);
	EA_ASSERT(Resources[0] == nullptr);

	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommittedResource(
		&ResourceCreateProperties.HeapProperties,
		ResourceCreateProperties.HeapFlags,
		&Desc,
		ResourceCreateProperties.InitialResourceStates,
		ResourceCreateProperties.ClearValue.has_value() ? &(ResourceCreateProperties.ClearValue.value()) : nullptr,
		IID_PPV_ARGS(&Resources[0])));
}

void FD3D12Resource::ReleaseResource()
{
	for (ComPtr<ID3D12Resource>& Resource : Resources)
	{
		Resource.Reset();
	}
}

#if D3D_NAME_OBJECT
void FD3D12Resource::SetDebugNameToResource(const wchar_t* const InDebugName)
{
	bool bAny = false;
	for (uint32_t Index = 0; Index < Resources.size(); ++Index)
	{
		if (Resources[Index])
		{
			Resources[Index]->SetName(InDebugName);
			bAny = true;
		}
	}
	EA_ASSERT(bAny);
}
#endif

void FD3D12Resource::ValidateResourceProperties() const
{
// 	if (Desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
// 	{
// 		EA_ASSERT(ResourceCreateProperties.ClearValue != nullptr);
// 	}
	
	if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
	{
		EA_ASSERT(!(ResourceCreateProperties.ClearValue.has_value()));
	}
}

D3D12_GPU_VIRTUAL_ADDRESS FD3D12Resource::GPUVirtualAddress() const
{
	EA_ASSERT(bInit);

	return GetResource()->GetGPUVirtualAddress();
}

FD3D12ConstantBufferView* FD3D12Resource::GetCBV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(IsBuffer());

	if (DefaultCBV == nullptr)
	{
		DefaultCBV = eastl::make_shared<FD3D12ConstantBufferView>(weak_from_this());
		DefaultCBV->UpdateDescriptor();
	}

	return DefaultCBV.get();
}

FD3D12ShaderResourceView* FD3D12Resource::GetSRV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(!(Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE));

	if (DefaultSRV == nullptr)
	{
		if (IsBuffer())
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc{};

			if (!Info.bNullView)
			{
				SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				SRVDesc.Format = UE::DXGIUtilities::FindShaderResourceFormat(DXGI_FORMAT(GPixelFormats[Info.Format].PlatformFormat), false);
				SRVDesc.Buffer.FirstElement = (Info.OffsetInBytes + Buffer->ResourceLocation.GetOffsetFromBaseOfResource()) / Info.StrideInBytes;
				SRVDesc.Buffer.NumElements = Info.NumElements;

				switch (Info.BufferType)
				{
				case FRHIViewDesc::EBufferType::Raw:
					SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					break;

				case FRHIViewDesc::EBufferType::Structured:
					SRVDesc.Buffer.StructureByteStride = Info.StrideInBytes;
					break;

				case FRHIViewDesc::EBufferType::Typed:
					// Nothing more to specify
					break;
				}
			}

			DefaultSRV = eastl::make_shared<FD3D12ShaderResourceView>(weak_from_this(), SRVDesc);
		}
		else
		{
			DefaultSRV = eastl::make_shared<FD3D12ShaderResourceView>(weak_from_this());
		}

		DefaultSRV->UpdateDescriptor();
	}

	return DefaultSRV.get();
}

FD3D12UnorderedAccessView* FD3D12Resource::GetUAV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	if (DefaultUAV == nullptr)
	{
		DefaultUAV = eastl::make_shared<FD3D12UnorderedAccessView>(weak_from_this());
		DefaultUAV->UpdateDescriptor();
	}

	return DefaultUAV.get();
}
																			
FD3D12RenderTargetView* FD3D12Resource::GetRTV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	if (DefaultRTV == nullptr)
	{
		DefaultRTV = eastl::make_shared<FD3D12RenderTargetView>(weak_from_this());
		DefaultRTV->UpdateDescriptor();
	}

	return DefaultRTV.get();
}

FD3D12DepthStencilView* FD3D12Resource::GetDSV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	if (DefaultDSV == nullptr)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
		DSVDesc.Format = ResourceCreateProperties.ClearValue->Format;
		EA_ASSERT(Desc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D);
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;

		DefaultDSV = eastl::make_shared<FD3D12DepthStencilView>(weak_from_this(), DSVDesc);
		DefaultDSV->UpdateDescriptor();
	}

	return DefaultDSV.get();
}

FD3D12TextureResource::FD3D12TextureResource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12Resource(InResource, InResourceCreateProperties, InDesc), ResourcePoolBlock()
{
}

FD3D12TextureResource::FD3D12TextureResource(ComPtr<ID3D12Resource>& InResource, const FD3D12ResourcePoolBlock& InResourcePoolBlock, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12Resource(InResource, InResourceCreateProperties, InDesc), ResourcePoolBlock(InResourcePoolBlock)
{

}

FD3D12TextureResource::FD3D12TextureResource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12Resource(InResourceCreateProperties, InDesc), ResourcePoolBlock()
{

}

FD3D12TextureResource::FD3D12TextureResource(ComPtr<ID3D12Resource> InRenderTargetResource)
	: FD3D12Resource(InRenderTargetResource)
{

}

FD3D12TextureResource::~FD3D12TextureResource()
{
	if (ResourcePoolBlock.has_value())
	{
		ResourcePoolBlock->FreeBlock();
	}
}

FD3D12Texture2DResource::FD3D12Texture2DResource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12TextureResource(InResource, InResourceCreateProperties, InDesc)
{

}

FD3D12Texture2DResource::FD3D12Texture2DResource(const FResourceCreateProperties& InResourceCreateProperties,
	const DXGI_FORMAT InFormat, const uint64_t InWidth, const uint32_t InHeight, 
	const uint16_t InArraySize /*= 1*/, const uint16_t InMipLevels /*= 0*/, const uint32_t InSampleCount /*= 1*/, 
	const uint32_t InSampleQuality /*= 0*/, const D3D12_RESOURCE_FLAGS InFlags /*= D3D12_RESOURCE_FLAG_NONE*/, 
	const D3D12_TEXTURE_LAYOUT InLayout /*= D3D12_TEXTURE_LAYOUT_UNKNOWN*/, const uint64_t InAlignment /*= 0 */)
	: FD3D12TextureResource(InResourceCreateProperties, 
		CD3DX12_RESOURCE_DESC::Tex2D(InFormat, InWidth, InHeight, InArraySize, InMipLevels, InSampleCount, InSampleQuality, InFlags, InLayout, InAlignment))
{

}

FD3D12Texture2DResource::FD3D12Texture2DResource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12TextureResource(InResourceCreateProperties, InDesc)
{

}

FD3D12Texture2DResource::FD3D12Texture2DResource(ComPtr<ID3D12Resource>& InResource, const FD3D12ResourcePoolBlock& InResourcePoolBlock, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: FD3D12TextureResource(InResource, InResourcePoolBlock, InResourceCreateProperties, InDesc)
{

}

FD3D12Texture2DResource::FD3D12Texture2DResource(ComPtr<ID3D12Resource> InRenderTargetResource, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat, const uint32_t InSampleCount, const uint32_t InSampleQuality)
	: FD3D12TextureResource(InRenderTargetResource)
{
	Desc.Width = InWidth;
	Desc.Height = InHeight;
	Desc.Format = InFormat;
	Desc.SampleDesc.Count = InSampleCount;
	Desc.SampleDesc.Quality = InSampleQuality;
}


FD3D12BufferResource::FD3D12BufferResource(
	const uint64_t InSize, const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment, const bool bInDynamic, const D3D12_RESOURCE_STATES InInitialResourceState,
	 uint8_t* const InShadowDataAddress, const uint32_t InShadowDataSize, const bool bNeverCreateShadowData)
	: 
	FD3D12Resource(MakeResourceCreateProperties(bInDynamic, InInitialResourceState), CD3DX12_RESOURCE_DESC::Buffer(InSize, InFlags, InAlignment)),
	bDynamic(bInDynamic), MappedAddress(nullptr), ShadowDataAddress(InShadowDataAddress), ShadowDataSize(InShadowDataSize), bIsShadowDataDirtyPerFrame{ true }
{
	if (ShadowDataAddress)
	{
		EA_ASSERT(ShadowDataSize > 0);
	}

	if (!ShadowDataAddress && !bNeverCreateShadowData)
	{
		ShadowData.resize(GetBufferSize());
		ShadowDataAddress = ShadowData.data();

		bShadowDataCreatedFromThisInstance = true;
	}
	else
	{
		bShadowDataCreatedFromThisInstance = false;
	}
}

FD3D12BufferResource::FResourceCreateProperties FD3D12BufferResource::MakeResourceCreateProperties(const bool bDynamic, const D3D12_RESOURCE_STATES InInitialResourceState) const
{
	FResourceCreateProperties ResourceCreateProperties{};

	// if dynamic, use upload heap for gpu
	ResourceCreateProperties.HeapProperties = CD3DX12_HEAP_PROPERTIES(bDynamic ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
	ResourceCreateProperties.HeapFlags = D3D12_HEAP_FLAG_NONE;
	ResourceCreateProperties.InitialResourceStates = InInitialResourceState; // if not dynamic, set D3D12_RESOURCE_STATE_COPY_DEST to initial state for upload

	return ResourceCreateProperties;
}

FD3D12VertexIndexBufferResource::FD3D12VertexIndexBufferResource(const uint64_t InSize, const uint32_t InDefaultStrideInBytes, const bool bInVertexBuffer, const bool bInDynamic /*= false*/)
	: FD3D12BufferResource(
		InSize, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
		0,
		bInDynamic, 
		bInDynamic ? D3D12_RESOURCE_STATE_GENERIC_READ : (bInVertexBuffer ? D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER : D3D12_RESOURCE_STATE_INDEX_BUFFER),
		nullptr, 
		true
	), 
	DefaultStrideInBytes(InDefaultStrideInBytes)
{

}

D3D12_VERTEX_BUFFER_VIEW FD3D12VertexIndexBufferResource::GetVertexBufferView() const
{
	return GetVertexBufferView(DefaultStrideInBytes);
}

D3D12_VERTEX_BUFFER_VIEW FD3D12VertexIndexBufferResource::GetVertexBufferView(const uint32_t InStrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW View{};

	View.BufferLocation = GPUVirtualAddress();
	View.SizeInBytes = Desc.Width;
	View.StrideInBytes = InStrideInBytes;

	return View;
}

D3D12_VERTEX_BUFFER_VIEW FD3D12VertexIndexBufferResource::GetVertexBufferView(const uint64_t InBaseOffsetInBytes, const uint32_t InSizeInBytes, const uint32_t InStrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW View{};

	View.BufferLocation = GPUVirtualAddress() + InBaseOffsetInBytes;
	View.SizeInBytes = InSizeInBytes;
	View.StrideInBytes = InStrideInBytes;

	return View;
}

D3D12_INDEX_BUFFER_VIEW FD3D12VertexIndexBufferResource::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW View;
	View.BufferLocation = GPUVirtualAddress();
	View.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	View.SizeInBytes = Desc.Width;

	return View;
}

D3D12_INDEX_BUFFER_VIEW FD3D12VertexIndexBufferResource::GetIndexBufferView(const uint64_t InBaseOffsetInBytes, const DXGI_FORMAT InFormat, const uint32_t InSizeInBytes) const
{
	D3D12_INDEX_BUFFER_VIEW View;
	View.BufferLocation = GPUVirtualAddress() + InBaseOffsetInBytes;
	View.Format = InFormat;
	View.SizeInBytes = InSizeInBytes;

	return View;
}

void FD3D12BufferResource::InitResource()
{
	FD3D12Resource::InitResource();
}

void FD3D12BufferResource::CreateD3D12Resource()
{
	FD3D12Resource::CreateD3D12Resource();

	if (bDynamic)
	{
		Map();
	}
}

void FD3D12BufferResource::ReleaseResource()
{
	FD3D12Resource::ReleaseResource();

	// Doesn't need unmap mapped resource. It's unmapped implcitly when destory resource
	//if (MappedAddress)
	//{
	//	GetResource()->Unmap(0, nullptr);
	//	MappedAddress = nullptr;
	//}
}

uint8_t* FD3D12BufferResource::GetMappedAddress() const
{
	EA_ASSERT(MappedAddress != nullptr);

	return MappedAddress;
}

uint8_t* FD3D12BufferResource::Map()
{
	EA_ASSERT(MappedAddress == nullptr);

	CD3DX12_RANGE ReadRange(0, 0);

	// Doesn't require unmap for being visible to gpu
	VERIFYD3D12RESULT(GetResource()->Map(0, &ReadRange, reinterpret_cast<void**>(&MappedAddress)));

	return MappedAddress;
}

void FD3D12BufferResource::Unmap()
{
	GetResource()->Unmap(0, nullptr);
	MappedAddress = nullptr;
}

uint8_t* FD3D12BufferResource::GetShadowDataAddress()
{
	EA_ASSERT(ShadowDataAddress);
	return ShadowDataAddress;
}

uint32_t FD3D12BufferResource::GetShadowDataSize() const
{
	return ShadowDataSize;
}

void FD3D12BufferResource::FlushShadowData()
{
	if (IsDynamicBuffer())
	{
		// memcpy is recommended to write on write combined type page. it's for preventing from flushing write combined buffer before fill up it
		EA_ASSERT(ShadowDataSize > 0);
		EA::StdC::Memcpy(GetMappedAddress(), GetShadowDataAddress(), ShadowDataSize); // d3d resource size can be different with shadow data size because of alignment requirement
	}
	else
	{
		FD3D12ResourceUpload ResourceUpload{};
		ResourceUpload.Resource = GetResource(GCurrentBackbufferIndex);
		EA_ASSERT(ResourceUpload.Resource);
		ResourceUpload.SubresourceContainers.emplace_back(eastl::make_unique<FD3D12ConstantBufferSubresourceContainer>(GetShadowDataAddress(), GetShadowDataSize()));
		ResourceUpload.ResourceBarriersBeforeUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(ResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));
		ResourceUpload.ResourceBarriersAfterUpload.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(ResourceUpload.Resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.AddPendingResourceUpload(eastl::move(ResourceUpload));
	}

	bIsShadowDataDirtyPerFrame[GCurrentBackbufferIndex] = false;
}

void FD3D12ConstantBufferResource::InitResource()
{
	FD3D12BufferResource::InitResource();
}

void FD3D12ConstantBufferResource::CreateD3D12Resource()
{
	EA_ASSERT(bInit);
	for (uint32_t Index = 0; Index < (bNeedVersioning ? Resources.size() : 1); ++Index)
	{
		EA_ASSERT(Resources[Index] == nullptr);

		VERIFYD3D12RESULT(GetD3D12Device()->CreateCommittedResource(
			&ResourceCreateProperties.HeapProperties,
			ResourceCreateProperties.HeapFlags,
			&Desc,
			ResourceCreateProperties.InitialResourceStates,
			ResourceCreateProperties.ClearValue.has_value() ? &(ResourceCreateProperties.ClearValue.value()) : nullptr,
			IID_PPV_ARGS(&Resources[Index])));
	}

	if (bDynamic)
	{
		Map();
	}
}

void FD3D12ConstantBufferResource::MakeDirty()
{
	for (bool& bIsShadowDataDirty : bIsShadowDataDirtyPerFrame)
	{
		// make dirty for all frame
		bIsShadowDataDirty = true;
	}
}

void FD3D12ConstantBufferResource::Versioning()
{
	EA_ASSERT(bNeedVersioning);
	if (IsDynamicBuffer())
	{
		ConstantBufferRingBufferBlock = FD3D12ConstantBufferRingBuffer::GetInstance()->Allocate(GetBufferSize());

		MappedAddress = ConstantBufferRingBufferBlock.MappedAddress;
	}
	else
	{
		EA_ASSERT(false);
	}
}

D3D12_GPU_VIRTUAL_ADDRESS FD3D12ConstantBufferResource::GPUVirtualAddress() const
{
	ID3D12Resource* const TargetResource = bNeedVersioning ? GetResource(GCurrentBackbufferIndex) : GetResource(0);

	if (TargetResource)
	{
		return TargetResource->GetGPUVirtualAddress();
	}
	else
	{
		EA_ASSERT(IsDynamicBuffer());
		EA_ASSERT(ConstantBufferRingBufferBlock.GPUVirtualAddress != 0);
		return ConstantBufferRingBufferBlock.GPUVirtualAddress;
	}
}