#include "D3D12Resource.h"

#include "D3D12Device.h"
#include "D3D12PerFrameConstantBufferManager.h"

FD3D12Resource::FD3D12Resource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: Fence(), ResourceCreateProperties(InResourceCreateProperties), Desc(InDesc), bInit(false), Resource(),
	DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
	ValidateResourceProperties();
}

FD3D12Resource::FD3D12Resource(ComPtr<ID3D12Resource>& InResource)
	: Fence(), ResourceCreateProperties(), bInit(false), Resource(InResource),
	DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
	Desc = CD3DX12_RESOURCE_DESC{ Resource->GetDesc() };

	Resource->GetHeapProperties(&ResourceCreateProperties.HeapProperties, &ResourceCreateProperties.HeapFlags);

	ValidateResourceProperties();
}

FD3D12Resource::FD3D12Resource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc)
	: Fence(), ResourceCreateProperties(InResourceCreateProperties), Desc(InDesc), bInit(false), Resource(InResource),
	DefaultSRV(), DefaultUAV(), DefaultRTV(), DefaultDSV()
{
}

void FD3D12Resource::InitResource()
{
	bInit = true;

	if (IsCreateD3D12ResourceOnInitResource() && !Resource)
	{
		CreateD3D12Resource();
	}
}

void FD3D12Resource::CreateD3D12Resource()
{
	EA_ASSERT(bInit);
	EA_ASSERT(Resource == nullptr);

	// todo : use placed resource
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommittedResource(
		&ResourceCreateProperties.HeapProperties,
		ResourceCreateProperties.HeapFlags,
		&Desc,
		ResourceCreateProperties.InitialResourceStates,
		ResourceCreateProperties.ClearValue.has_value() ? &(ResourceCreateProperties.ClearValue.value()) : nullptr,
		IID_PPV_ARGS(&Resource)));
}

void FD3D12Resource::ClearResource()
{
}

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

// FD3D12ConstantBufferView* FD3D12Resource::GetCBV()
// {
// 	EA_ASSERT(bInit);
// 	EA_ASSERT(IsBuffer());
// 
// 	if (DefaultCBV == nullptr)
// 	{
// 		DefaultCBV = eastl::make_shared<FD3D12ConstantBufferView>(this);
// 		DefaultCBV->UpdateDescriptor();
// 	}
// 
// 	return DefaultCBV.get();
// }

FD3D12ShaderResourceView* FD3D12Resource::GetSRV()
{
	EA_ASSERT(bInit);
	EA_ASSERT(!(Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE));

	if (DefaultSRV == nullptr)
	{
		DefaultSRV = eastl::make_shared<FD3D12ShaderResourceView>(this);
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
		DefaultUAV = eastl::make_shared<FD3D12UnorderedAccessView>(this);
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
		DefaultRTV = eastl::make_shared<FD3D12RenderTargetView>(this);
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
		DefaultDSV = eastl::make_shared<FD3D12DepthStencilView>(this);
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

FD3D12BufferResource::FD3D12BufferResource(
	const uint64_t InSize, const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment, const bool bInDynamic, uint8_t* const InShadowDataAddress, const bool bNeverCreateShadowData)
	: 
	FD3D12Resource(MakeResourceCreateProperties(bDynamic), CD3DX12_RESOURCE_DESC::Buffer(InSize, InFlags, InAlignment)),
	bDynamic(bInDynamic), MappedAddress(nullptr), ShadowDataAddress(InShadowDataAddress)
{
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

D3D12_VERTEX_BUFFER_VIEW FD3D12BufferResource::GetVertexBufferView(const uint32_t InStrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW View{};

	View.BufferLocation = GPUVirtualAddress();
	View.SizeInBytes = Desc.Width;
	View.StrideInBytes = InStrideInBytes;

	return View;
}

D3D12_VERTEX_BUFFER_VIEW FD3D12BufferResource::GetVertexBufferView(const uint64_t InBaseOffsetInBytes, const uint32_t InSizeInBytes, const uint32_t InStrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW View{};

	View.BufferLocation = GPUVirtualAddress() + InBaseOffsetInBytes;
	View.SizeInBytes = InSizeInBytes;
	View.StrideInBytes = InStrideInBytes;

	return View;
}

D3D12_INDEX_BUFFER_VIEW FD3D12BufferResource::GetIndexBufferView(const uint64_t InBaseOffsetInBytes, const DXGI_FORMAT InFormat, const uint32_t InSizeInBytes) const
{
	D3D12_INDEX_BUFFER_VIEW View;
	View.BufferLocation = GPUVirtualAddress() + InBaseOffsetInBytes;
	View.Format = InFormat;
	View.SizeInBytes = InSizeInBytes;

	return View;
}

FD3D12BufferResource::FResourceCreateProperties FD3D12BufferResource::MakeResourceCreateProperties(const bool bDynamic) const
{
	FResourceCreateProperties ResourceCreateProperties{};

	// if dynamic, use upload heap for gpu
	ResourceCreateProperties.HeapProperties = CD3DX12_HEAP_PROPERTIES(bDynamic ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
	ResourceCreateProperties.HeapFlags = D3D12_HEAP_FLAG_NONE;
	ResourceCreateProperties.InitialResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;

	return ResourceCreateProperties;
}

FD3D12VertexIndexBufferResource::FD3D12VertexIndexBufferResource(const uint64_t InSize, const bool bInDynamic /*= false*/)
	: FD3D12BufferResource(InSize, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE, 0, bInDynamic, nullptr, true)
{

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

void FD3D12BufferResource::ClearResource()
{
	FD3D12Resource::ClearResource();

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

void FD3D12BufferResource::FlushShadowData()
{
	if (bDynamic)
	{
		// memcpy is recommended to write on write combined type page. it's for preventing from flushing write combined buffer before fill up it
		EA::StdC::Memcpy(GetMappedAddress(), GetShadowDataAddress(), GetBufferSize());
	}
	else
	{
		// @todo : upload to gpu
	}
}

void FD3D12ConstantBufferResource::InitResource()
{
	FD3D12BufferResource::InitResource();
}

void FD3D12ConstantBufferResource::Versioning()
{
	EA_ASSERT(IsDynamicBuffer()); // todo : support no-dynamic buffer

	ConstantBufferBlock = FD3D12PerFrameConstantBufferManager::GetInstance()->Allocate(GetBufferSize());

	MappedAddress = ConstantBufferBlock.MappedAddress;
}

D3D12_GPU_VIRTUAL_ADDRESS FD3D12ConstantBufferResource::GPUVirtualAddress() const
{
	if (GetResource())
	{
		return GetResource()->GetGPUVirtualAddress();
	}
	else
	{
		EA_ASSERT(ConstantBufferBlock.GPUVirtualAddress != 0);
		return ConstantBufferBlock.GPUVirtualAddress;
	}
}

FD3D12RenderTargetResource::FD3D12RenderTargetResource(ComPtr<ID3D12Resource> InRenderTargetResource)
	: FD3D12Resource(InRenderTargetResource)
{

}