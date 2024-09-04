#include "D3D12View.h"
#include "D3D12Device.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"

FD3D12View::~FD3D12View()
{
	FreeDescriptorHeapBlock();
}

FD3D12Resource* FD3D12View::GetUnderlyingResource() const
{
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	return LockedResource ? LockedResource.get() : nullptr;
}

const FD3D12DescriptorHeapBlock& FD3D12View::GetDescriptorHeapBlock() const
{
	return OfflineDescriptorHeapBlock;
}

void FD3D12View::FreeDescriptorHeapBlock()
{
	if (eastl::shared_ptr<FD3D12DescriptorHeap> OfflineDescriptorHeap = OfflineDescriptorHeapBlock.ParentDescriptorHeap.lock())
	{
		OfflineDescriptorHeap->FreeDescriptorHeapBlock(OfflineDescriptorHeapBlock);
	}
	OfflineDescriptorHeapBlock.Clear();
}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(eastl::weak_ptr<FD3D12Resource> InResource)
	: TD3D12View(InResource)
{
}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc)
	: TD3D12View(InDesc)
{

}

void FD3D12ShaderResourceView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	GetD3D12Device()->CreateShaderResourceView(LockedResource ? LockedResource->GetResource() : nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12ShaderResourceView* FD3D12ShaderResourceView::NullSRV()
{
	static eastl::unique_ptr<FD3D12ShaderResourceView> DefaultSRV = nullptr;
	if (DefaultSRV == nullptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC NullSRVDesc{};
		NullSRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		NullSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		NullSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		NullSRVDesc.Texture2D.MipLevels = 1;
		NullSRVDesc.Texture2D.MostDetailedMip = 0;
		NullSRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		DefaultSRV = eastl::make_unique<FD3D12ShaderResourceView>(NullSRVDesc);
		DefaultSRV->UpdateDescriptor();
	}
	return DefaultSRV.get();
}

 FD3D12ConstantBufferView::FD3D12ConstantBufferView(eastl::weak_ptr<FD3D12Resource> InResource)
 	: TD3D12View(InResource)
 {
 }

 FD3D12ConstantBufferView::FD3D12ConstantBufferView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_CONSTANT_BUFFER_VIEW_DESC& InDesc)
	 : TD3D12View(InResource, InDesc)
 {

 }

 FD3D12ConstantBufferView::FD3D12ConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC& InDesc)
	 : TD3D12View(InDesc)
 {

 }

 FD3D12ConstantBufferView* FD3D12ConstantBufferView::NullCBV()
 {
	 static eastl::unique_ptr<FD3D12ConstantBufferView> DefaultCBV = nullptr;
	 if (DefaultCBV == nullptr)
	 {
		 D3D12_CONSTANT_BUFFER_VIEW_DESC NullCBVDesc{};
		 DefaultCBV = eastl::make_unique<FD3D12ConstantBufferView>(NullCBVDesc);
		 DefaultCBV->UpdateDescriptor();
	 }
	 return DefaultCBV.get();
 }
 
 void FD3D12ConstantBufferView::UpdateDescriptor()
 {
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
 
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = {};
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	CBVDesc.BufferLocation = LockedResource->GPUVirtualAddress();
	CBVDesc.SizeInBytes = Align(LockedResource->GetDesc().Width, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT); // Constant buffers must be 256 byte aligned, which is different than the alignment requirement for a resource heap
 
	GetD3D12Device()->CreateConstantBufferView(&CBVDesc, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
 }

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(eastl::weak_ptr<FD3D12Resource> InResource)
	: TD3D12View(InResource)
{

}

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
	: TD3D12View(InDesc)
{

}

void FD3D12UnorderedAccessView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);

	// @todo : support counter resource
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	GetD3D12Device()->CreateUnorderedAccessView(LockedResource ? LockedResource->GetResource() : nullptr, nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12UnorderedAccessView* FD3D12UnorderedAccessView::NullUAV()
{
	static eastl::unique_ptr<FD3D12UnorderedAccessView> DefaultUAV = nullptr;
	if (DefaultUAV == nullptr)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC NullUAVDesc{};
		NullUAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		NullUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		NullUAVDesc.Texture2D.MipSlice = 0;

		DefaultUAV = eastl::make_unique<FD3D12UnorderedAccessView>(NullUAVDesc);
		DefaultUAV->UpdateDescriptor();
	}
	return DefaultUAV.get();
}

FD3D12RenderTargetView::FD3D12RenderTargetView(eastl::weak_ptr<FD3D12Resource> InResource)
	: TD3D12View(InResource)
{

}

FD3D12RenderTargetView::FD3D12RenderTargetView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{
}

FD3D12RenderTargetView::FD3D12RenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& InDesc)
	: TD3D12View(InDesc)
{

}

void FD3D12RenderTargetView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->RTVDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	GetD3D12Device()->CreateRenderTargetView(LockedResource ? LockedResource->GetResource() : nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12RenderTargetView* FD3D12RenderTargetView::NullRTV()
{
	static eastl::unique_ptr<FD3D12RenderTargetView> DefaultRTV = nullptr;
	if (DefaultRTV == nullptr)
	{
		// @TODO : Do we need the version for D3D12_RTV_DIMENSION_TEXTURE3D
		D3D12_RENDER_TARGET_VIEW_DESC NullRTVDesc{};
		NullRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		NullRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		NullRTVDesc.Texture2D.MipSlice = 0;

		DefaultRTV = eastl::make_unique<FD3D12RenderTargetView>(NullRTVDesc);
		DefaultRTV->UpdateDescriptor();
	}
	return DefaultRTV.get();
}

FD3D12DepthStencilView::FD3D12DepthStencilView(eastl::weak_ptr<FD3D12Resource> InResource)
	: TD3D12View(InResource)
{

}

FD3D12DepthStencilView::FD3D12DepthStencilView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

FD3D12DepthStencilView::FD3D12DepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc)
	: TD3D12View(InDesc)
{

}

void FD3D12DepthStencilView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->DSVDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	eastl::shared_ptr<FD3D12Resource> LockedResource = Resource.lock();
	GetD3D12Device()->CreateDepthStencilView(LockedResource ? LockedResource->GetResource() : nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12DepthStencilView* FD3D12DepthStencilView::NullDSV()
{
	static eastl::unique_ptr<FD3D12DepthStencilView> DefaultDSV = nullptr;
	if (DefaultDSV == nullptr)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC NullDSVDesc{};
		NullDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		NullDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		NullDSVDesc.Texture2D.MipSlice = 0;

		DefaultDSV = eastl::make_unique<FD3D12DepthStencilView>(NullDSVDesc);
		DefaultDSV->UpdateDescriptor();
	}
	return DefaultDSV.get();
}
