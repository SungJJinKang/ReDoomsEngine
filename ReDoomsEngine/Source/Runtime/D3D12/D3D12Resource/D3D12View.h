#pragma once
#include "D3D12Include.h"

#include "D3D12Descriptor.h"
#include "EASTL/optional.h"

class FD3D12Resource;
class FD3D12RenderTargetResource;
class FD3D12DepthStencilTargetResource;

template <typename DescType>
class FD3D12View
{
public:
	
	FD3D12View(FD3D12Resource* const InResource)
		: OfflineDescriptorHeapBlock(), Resource(InResource), Desc()
	{

	}
	FD3D12View(FD3D12Resource* const InResource, const DescType& InDesc)
		: OfflineDescriptorHeapBlock(), Resource(InResource), Desc(InDesc)
	{

	}
	virtual ~FD3D12View()
	{
		FreeDescriptorHeapBlock();
	}

	virtual void UpdateDescriptor() = 0;
	void FreeDescriptorHeapBlock()
	{
		if (eastl::shared_ptr<FD3D12DescriptorHeap> OfflineDescriptorHeap = OfflineDescriptorHeapBlock.ParentDescriptorHeap.lock())
		{
			OfflineDescriptorHeap->FreeDescriptorHeapBlock(OfflineDescriptorHeapBlock);
		}
	}

	inline FD3D12Resource* GetUnderlyingResource() const
	{
		return Resource;
	}

	inline eastl::optional<DescType> GetDesc() const
	{
		return Desc;
	}

	inline FD3D12DescriptorHeapBlock GetDescriptorHeapBlock() const
	{
		return OfflineDescriptorHeapBlock;
	}

protected:

	FD3D12DescriptorHeapBlock OfflineDescriptorHeapBlock;

	FD3D12Resource* Resource;
	eastl::optional<DescType> Desc;
};

class FD3D12ShaderResourceView : public FD3D12View<D3D12_SHADER_RESOURCE_VIEW_DESC>
{
public:

	FD3D12ShaderResourceView(FD3D12Resource* const InResource);
	FD3D12ShaderResourceView(FD3D12Resource* const InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

protected:


private:

};

class FD3D12ConstantBufferView : public FD3D12View<D3D12_CONSTANT_BUFFER_VIEW_DESC>
{
public:

	FD3D12ConstantBufferView(FD3D12Resource* const InResource);

	virtual void UpdateDescriptor();

protected:


private:

};


class FD3D12UnorderedAccessView : public FD3D12View<D3D12_UNORDERED_ACCESS_VIEW_DESC>
{
public:

	FD3D12UnorderedAccessView(FD3D12Resource* const InResource);
	FD3D12UnorderedAccessView(FD3D12Resource* const InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

protected:


private:

};

class FD3D12RenderTargetView : public FD3D12View<D3D12_RENDER_TARGET_VIEW_DESC>
{
public:

	FD3D12RenderTargetView(FD3D12Resource* const InResource);
	FD3D12RenderTargetView(FD3D12Resource* const InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

protected:

private:

};

class FD3D12DepthStencilView : public FD3D12View<D3D12_DEPTH_STENCIL_VIEW_DESC>
{
public:

	FD3D12DepthStencilView(FD3D12Resource* const InResource);
	FD3D12DepthStencilView(FD3D12Resource* const InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

protected:


private:
};