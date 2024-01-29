#pragma once
#include "D3D12Include.h"

#include "D3D12Descriptor.h"

template <typename ViewType, typename DescType>
struct FD3D12View
{
	ResourceViewType* View;
	DescType Desc;

	FD3D12OfflineDescriptor OfflineDescriptor;
};

struct FD3D12ShaderResourceView : public FD3D12View<nullptr, D3D12_RENDER_TARGET_VIEW_DESC>
{

};

struct FD3D12UnorderedAccessView
{

};

struct FD3D12RenderTargetView
{

};

struct FD3D12DepthStencilView
{

};