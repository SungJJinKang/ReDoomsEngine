#include "D3D12View.h"
#include "D3D12Device.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"

FD3D12View::~FD3D12View()
{
	FreeDescriptorHeapBlock();
}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(FD3D12Resource* const InResource)
	: TD3D12View(InResource)
{
}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(FD3D12Resource* const InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

void FD3D12ShaderResourceView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	GetD3D12Device()->CreateShaderResourceView(Resource->GetResource(), Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
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

		DefaultSRV = eastl::make_unique<FD3D12ShaderResourceView>(nullptr, NullSRVDesc);
		DefaultSRV->UpdateDescriptor();
	}
	return DefaultSRV.get();
}

 FD3D12ConstantBufferView::FD3D12ConstantBufferView(FD3D12Resource* const InResource)
 	: TD3D12View(InResource)
 {
 }

 FD3D12ConstantBufferView::FD3D12ConstantBufferView(FD3D12Resource* const InResource, const D3D12_CONSTANT_BUFFER_VIEW_DESC& InDesc)
	 : TD3D12View(InResource, InDesc)
 {

 }

 FD3D12ConstantBufferView* FD3D12ConstantBufferView::NullCBV()
 {
	 static eastl::unique_ptr<FD3D12ConstantBufferView> DefaultCBV = nullptr;
	 if (DefaultCBV == nullptr)
	 {
		 D3D12_CONSTANT_BUFFER_VIEW_DESC NullCBVDesc{};
		 DefaultCBV = eastl::make_unique<FD3D12ConstantBufferView>(nullptr, NullCBVDesc);
		 DefaultCBV->UpdateDescriptor();
	 }
	 return DefaultCBV.get();
 }
 
 void FD3D12ConstantBufferView::UpdateDescriptor()
 {
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
 
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = {};
	CBVDesc.BufferLocation = Resource->GPUVirtualAddress();
	CBVDesc.SizeInBytes = Align(Resource->GetDesc().Width, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT); // Constant buffers must be 256 byte aligned, which is different than the alignment requirement for a resource heap
 
	GetD3D12Device()->CreateConstantBufferView(&CBVDesc, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
 }

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(FD3D12Resource* const InResource)
	: TD3D12View(InResource)
{

}

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(FD3D12Resource* const InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

void FD3D12UnorderedAccessView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOfflineDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);

	// @todo : support counter resource
	GetD3D12Device()->CreateUnorderedAccessView(Resource ? Resource->GetResource() : nullptr, nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
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

		DefaultUAV = eastl::make_unique<FD3D12UnorderedAccessView>(nullptr, NullUAVDesc);
		DefaultUAV->UpdateDescriptor();
	}
	return DefaultUAV.get();
}

FD3D12RenderTargetView::FD3D12RenderTargetView(FD3D12Resource* const InResource)
	: TD3D12View(InResource)
{

}

FD3D12RenderTargetView::FD3D12RenderTargetView(FD3D12Resource* const InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{
}

void FD3D12RenderTargetView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->RTVDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	GetD3D12Device()->CreateRenderTargetView(Resource ? Resource->GetResource() : nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12RenderTargetView* FD3D12RenderTargetView::NullRTV()
{
	static eastl::unique_ptr<FD3D12RenderTargetView> DefaultRTV = nullptr;
	if (DefaultRTV == nullptr)
	{
		D3D12_RENDER_TARGET_VIEW_DESC NullRTVDesc{};
		NullRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		NullRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		NullRTVDesc.Texture2D.MipSlice = 0;

		DefaultRTV = eastl::make_unique<FD3D12RenderTargetView>(nullptr, NullRTVDesc);
		DefaultRTV->UpdateDescriptor();
	}
	return DefaultRTV.get();
}

FD3D12DepthStencilView::FD3D12DepthStencilView(FD3D12Resource* const InResource)
	: TD3D12View(InResource)
{

}

FD3D12DepthStencilView::FD3D12DepthStencilView(FD3D12Resource* const InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc)
	: TD3D12View(InResource, InDesc)
{

}

void FD3D12DepthStencilView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->DSVDescriptorHeapContainer.AllocateDescriptorHeapBlock(1);
	GetD3D12Device()->CreateDepthStencilView(Resource ? Resource->GetResource() : nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
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

		DefaultDSV = eastl::make_unique<FD3D12DepthStencilView>(nullptr, NullDSVDesc);
		DefaultDSV->UpdateDescriptor();
	}
	return DefaultDSV.get();
}
