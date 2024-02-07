#pragma once
#include "D3D12Include.h"

#include "D3D12Descriptor.h"
#include "EASTL/optional.h"

class FD3D12Resource;
class FD3D12RenderTargetResource;
class FD3D12DepthStencilTargetResource;

class FD3D12View
{
public:

	FD3D12View(FD3D12Resource* const InResource)
		: OfflineDescriptorHeapBlock(), Resource(InResource)
	{

	}

	virtual ~FD3D12View()
	{
		FreeDescriptorHeapBlock();
	}

	inline FD3D12Resource* GetUnderlyingResource() const
	{
		return Resource;
	}

	inline FD3D12DescriptorHeapBlock GetDescriptorHeapBlock() const
	{
		return OfflineDescriptorHeapBlock;
	}

	virtual void UpdateDescriptor() = 0;
	void FreeDescriptorHeapBlock()
	{
		if (eastl::shared_ptr<FD3D12DescriptorHeap> OfflineDescriptorHeap = OfflineDescriptorHeapBlock.ParentDescriptorHeap.lock())
		{
			OfflineDescriptorHeap->FreeDescriptorHeapBlock(OfflineDescriptorHeapBlock);
		}
	}

	virtual bool IsSRV() const { return false; }
	virtual bool IsUAV() const { return false; }
	virtual bool IsRTV() const { return false; }
	virtual bool IsDSV() const { return false; }

protected:

	FD3D12DescriptorHeapBlock OfflineDescriptorHeapBlock;

	FD3D12Resource* Resource;
};

template <typename DescType>
class TD3D12View : public FD3D12View
{
public:
	
	TD3D12View(FD3D12Resource* const InResource)
		: FD3D12View(InResource), Desc()
	{

	}
	TD3D12View(FD3D12Resource* const InResource, const DescType& InDesc)
		: FD3D12View(InResource), Desc(InDesc)
	{

	}

	inline eastl::optional<DescType> GetDesc() const
	{
		return Desc;
	}

protected:

	eastl::optional<DescType> Desc;
};

class FD3D12ShaderResourceView : public TD3D12View<D3D12_SHADER_RESOURCE_VIEW_DESC>
{
public:

	FD3D12ShaderResourceView(FD3D12Resource* const InResource);
	FD3D12ShaderResourceView(FD3D12Resource* const InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

	virtual bool IsSRV() const { return true; }

protected:


private:

};

// class FD3D12ConstantBufferView : public TD3D12View<D3D12_CONSTANT_BUFFER_VIEW_DESC>
// {
// public:
// 
// 	FD3D12ConstantBufferView(FD3D12Resource* const InResource);
// 
// 	virtual void UpdateDescriptor();
// 
// protected:
// 
// 
// private:
// 
// };


class FD3D12UnorderedAccessView : public TD3D12View<D3D12_UNORDERED_ACCESS_VIEW_DESC>
{
public:

	FD3D12UnorderedAccessView(FD3D12Resource* const InResource);
	FD3D12UnorderedAccessView(FD3D12Resource* const InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

	virtual bool IsUAV() const { return true; }

protected:


private:

};

class FD3D12RenderTargetView : public TD3D12View<D3D12_RENDER_TARGET_VIEW_DESC>
{
public:

	FD3D12RenderTargetView(FD3D12Resource* const InResource);
	FD3D12RenderTargetView(FD3D12Resource* const InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

	virtual bool IsRTV() const { return true; }
protected:

private:

};

class FD3D12DepthStencilView : public TD3D12View<D3D12_DEPTH_STENCIL_VIEW_DESC>
{
public:

	FD3D12DepthStencilView(FD3D12Resource* const InResource);
	FD3D12DepthStencilView(FD3D12Resource* const InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();

	virtual bool IsDSV() const { return true; }

protected:


private:
};