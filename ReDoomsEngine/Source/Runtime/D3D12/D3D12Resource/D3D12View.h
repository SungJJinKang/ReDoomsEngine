#pragma once
#include "D3D12Include.h"

#include "D3D12Descriptor.h"
#include "EASTL/optional.h"

class FD3D12Resource;
class FD3D12Texture2DResource;
class FD3D12Texture2DResource;

class FD3D12View
{
public:

	FD3D12View(eastl::weak_ptr<FD3D12Resource> InResource)
		: OfflineDescriptorHeapBlock(), Resource(InResource)
	{

	}
	FD3D12View()
		: OfflineDescriptorHeapBlock(), Resource()
	{

	}

	virtual ~FD3D12View();

	FD3D12Resource* GetUnderlyingResource() const;

	FD3D12DescriptorHeapBlock GetDescriptorHeapBlock() const;

	virtual void UpdateDescriptor() = 0;
	void FreeDescriptorHeapBlock();

	virtual bool IsCBV() const { return false; }
	virtual bool IsSRV() const { return false; }
	virtual bool IsUAV() const { return false; }
	virtual bool IsRTV() const { return false; }
	virtual bool IsDSV() const { return false; }

protected:

	FD3D12DescriptorHeapBlock OfflineDescriptorHeapBlock;

	eastl::weak_ptr<FD3D12Resource> Resource;
};

template <typename DescType>
class TD3D12View : public FD3D12View
{
public:
	
	TD3D12View(eastl::weak_ptr<FD3D12Resource> InResource)
		: FD3D12View(InResource), Desc()
	{

	}
	TD3D12View(eastl::weak_ptr<FD3D12Resource> InResource, const DescType& InDesc)
		: FD3D12View(InResource), Desc(InDesc)
	{

	}
	TD3D12View(const DescType& InDesc)
		: FD3D12View(), Desc(InDesc)
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

	FD3D12ShaderResourceView(eastl::weak_ptr<FD3D12Resource> InResource);
	FD3D12ShaderResourceView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc);
	FD3D12ShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();
	static FD3D12ShaderResourceView* NullSRV();

	virtual bool IsSRV() const { return true; }

protected:


private:


};

 class FD3D12ConstantBufferView : public TD3D12View<D3D12_CONSTANT_BUFFER_VIEW_DESC>
 {
 public:
 
	 FD3D12ConstantBufferView(eastl::weak_ptr<FD3D12Resource> InResource);
	 FD3D12ConstantBufferView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_CONSTANT_BUFFER_VIEW_DESC& InDesc);
	 FD3D12ConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC& InDesc);
 
 	virtual void UpdateDescriptor();
	static FD3D12ConstantBufferView* NullCBV();

	virtual bool IsCBV() const { return true; }

 protected:
 
 
 private:
 
 };


class FD3D12UnorderedAccessView : public TD3D12View<D3D12_UNORDERED_ACCESS_VIEW_DESC>
{
public:

	FD3D12UnorderedAccessView(eastl::weak_ptr<FD3D12Resource> InResource);
	FD3D12UnorderedAccessView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);
	FD3D12UnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();
	static FD3D12UnorderedAccessView* NullUAV();

	virtual bool IsUAV() const { return true; }

protected:


private:

};

class FD3D12RenderTargetView : public TD3D12View<D3D12_RENDER_TARGET_VIEW_DESC>
{
public:

	FD3D12RenderTargetView(eastl::weak_ptr<FD3D12Resource> InResource);
	FD3D12RenderTargetView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_RENDER_TARGET_VIEW_DESC& InDesc);
	FD3D12RenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();
	static FD3D12RenderTargetView* NullRTV();

	virtual bool IsRTV() const { return true; }
protected:

private:

};

class FD3D12DepthStencilView : public TD3D12View<D3D12_DEPTH_STENCIL_VIEW_DESC>
{
public:

	FD3D12DepthStencilView(eastl::weak_ptr<FD3D12Resource> InResource);
	FD3D12DepthStencilView(eastl::weak_ptr<FD3D12Resource> InResource, const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc);
	FD3D12DepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& InDesc);

	virtual void UpdateDescriptor();
	static FD3D12DepthStencilView* NullDSV();

	virtual bool IsDSV() const { return true; }

protected:


private:
};