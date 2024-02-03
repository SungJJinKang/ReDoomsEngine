#include "D3D12View.h"
#include "D3D12Device.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"

FD3D12ShaderResourceView::FD3D12ShaderResourceView(FD3D12Resource* const InResource)
	: FD3D12View(InResource)
{
}

FD3D12ShaderResourceView::FD3D12ShaderResourceView(FD3D12Resource* const InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc)
	: FD3D12View(InResource, InDesc)
{

}

void FD3D12ShaderResourceView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->AllocateOfflineHeapDescriptorHeapBlock(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	GetD3D12Device()->CreateShaderResourceView(Resource->GetResource(), Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12ConstantBufferView::FD3D12ConstantBufferView(FD3D12Resource* const InResource)
	: FD3D12View(InResource)
{
}

void FD3D12ConstantBufferView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->AllocateOfflineHeapDescriptorHeapBlock(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = {};
	CBVDesc.BufferLocation = Resource->GPUVirtualAddress();
	CBVDesc.SizeInBytes = Align(Resource->GetDesc().Width, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT); // Constant buffers must be 256 byte aligned, which is different than the alignment requirement for a resource heap

	GetD3D12Device()->CreateConstantBufferView(&CBVDesc, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(FD3D12Resource* const InResource)
	: FD3D12View(InResource)
{

}

FD3D12UnorderedAccessView::FD3D12UnorderedAccessView(FD3D12Resource* const InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
	: FD3D12View(InResource, InDesc)
{

}

void FD3D12UnorderedAccessView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->AllocateOfflineHeapDescriptorHeapBlock(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

	// @todo : support counter resource
	GetD3D12Device()->CreateUnorderedAccessView(Resource->GetResource(), nullptr, Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12RenderTargetView::FD3D12RenderTargetView(FD3D12Resource* const InResource)
	: FD3D12View(InResource)
{

}

FD3D12RenderTargetView::FD3D12RenderTargetView(FD3D12Resource* const InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc)
	: FD3D12View(InResource, InDesc)
{
}

void FD3D12RenderTargetView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->AllocateOnlineHeapDescriptorHeapBlock(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
	GetD3D12Device()->CreateRenderTargetView(Resource->GetResource(), Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}

FD3D12DepthStencilView::FD3D12DepthStencilView(FD3D12Resource* const InResource)
	: FD3D12View(InResource)
{

}

FD3D12DepthStencilView::FD3D12DepthStencilView(FD3D12Resource* const InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc)
	: FD3D12View(InResource, InDesc)
{

}

void FD3D12DepthStencilView::UpdateDescriptor()
{
	OfflineDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->AllocateOnlineHeapDescriptorHeapBlock(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	GetD3D12Device()->CreateDepthStencilView(Resource->GetResource(), Desc.has_value() ? &(Desc.value()) : nullptr, OfflineDescriptorHeapBlock.CPUDescriptorHandle());
}