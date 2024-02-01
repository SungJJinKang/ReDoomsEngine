#pragma once
#include "D3D12Include.h"

#include "D3D12View.h"

class FD3D12Resource
{
public:

	struct FResourceCreateProperties
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties{};
		D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;
		D3D12_RESOURCE_STATES InitialResourceStates = D3D12_RESOURCE_STATE_COMMON;
		D3D12_CLEAR_VALUE* ClearValue = nullptr;
	};

	virtual void InitResource();
	virtual void ClearResource();
	void ValidateResourceProperties() const;

	inline const CD3DX12_HEAP_PROPERTIES& GetHeapProperties() const
	{
		return ResourceCreateProperties.HeapProperties;
	}
	inline D3D12_HEAP_FLAGS GetHeapFlags() const
	{
		return ResourceCreateProperties.HeapFlags;
	}
	inline const CD3DX12_RESOURCE_DESC& GetDesc() const
	{
		return Desc;
	}
	inline D3D12_CLEAR_VALUE* GetClearValue() const
	{
		return ResourceCreateProperties.ClearValue;
	}

	inline ID3D12Resource* GetResource() const 
	{
		return Resource.Get();
	}

	virtual bool IsBuffer() const = 0;
	virtual bool IsTexture() const = 0;

	FD3D12ConstantBufferView* GetCBV();
	FD3D12ShaderResourceView* GetSRV();
	FD3D12UnorderedAccessView* GetUAV();
	FD3D12RenderTargetView* GetRTV();
	FD3D12DepthStencilView* GetDSV();

protected:

	FD3D12Resource() = delete;
	FD3D12Resource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12Resource(ComPtr<ID3D12Resource> InResource);

	FResourceCreateProperties ResourceCreateProperties;
	CD3DX12_RESOURCE_DESC Desc;

private:

	ComPtr<ID3D12Resource> Resource;

	eastl::shared_ptr<FD3D12ConstantBufferView> DefaultCBV;
	eastl::shared_ptr<FD3D12ShaderResourceView> DefaultSRV;
	eastl::shared_ptr<FD3D12UnorderedAccessView> DefaultUAV;
	eastl::shared_ptr<FD3D12RenderTargetView> DefaultRTV;
	eastl::shared_ptr<FD3D12DepthStencilView> DefaultDSV;
};

class FD3D12TextureResource : public FD3D12Resource
{
public:

	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsTexture() const
	{
		return true;
	}

protected:
	FD3D12TextureResource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
};

class FD3D12Texture2DResource : public FD3D12TextureResource
{
	FD3D12Texture2DResource(
		const FResourceCreateProperties& InResourceCreateProperties,
		const DXGI_FORMAT InFormat,
		const uint64_t InWidth,
		const uint32_t InHeight,
		const uint16_t InArraySize = 1,
		const uint16_t InMipLevels = 0,
		const uint32_t InSampleCount = 1,
		const uint32_t InSampleQuality = 0,
		const D3D12_RESOURCE_FLAGS InFlags = D3D12_RESOURCE_FLAG_NONE,
		const D3D12_TEXTURE_LAYOUT InLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		const uint64_t InAlignment = 0
	);
};


class FD3D12BufferResource : public FD3D12Resource
{
public:

	FD3D12BufferResource(const uint64_t InSize, const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment = 0, const bool bInDynamic = false, uint8_t* const InShadowDataAddress = nullptr);
	
	virtual bool IsBuffer() const
	{
		return true;
	}
	virtual bool IsTexture() const
	{
		return false;
	}
	virtual bool IsConstantBuffer() const 
	{
		return false;
	}

	virtual void InitResource();
	virtual void ClearResource();

	inline bool IsDynamicBuffer() const 
	{
		return bDynamic;
	}

	uint64_t GetBufferSize() const
	{
		return Desc.Width;
	}

	/// <summary>
	/// Mapped address is write-combined type so it's recommended to copy data using memcpy style copy
	/// </summary>
	/// <returns></returns>
	uint8_t* GetMappedAddress() const;
	uint8_t* Map();
	void Unmap();

	uint8_t* GetShadowDataAddress();
	void FlushShadowData();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(const uint32_t InStrideInBytes) const;
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(const uint64_t InBaseOffsetInBytes, const uint32_t InSizeInBytes, const uint32_t InStrideInBytes) const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(const uint64_t InBaseOffsetInBytes, const DXGI_FORMAT InFormat, const uint32_t InSizeInBytes) const;

protected:

	FResourceCreateProperties MakeResourceCreateProperties(const bool bDynamic) const;

	bool bDynamic;
	uint8_t* MappedAddress = nullptr;

	bool bShadowDataCreatedFromThisInstance;
	uint8_t* ShadowDataAddress;

	eastl::vector<uint8_t> ShadowData;
};

template <typename BufferDataType>
class TD3D12BufferResource : public FD3D12BufferResource
{
public:
	
	TD3D12BufferResource(const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment = 0, const bool bInDynamic = false)
		: FD3D12BufferResource(sizeof(BufferDataType), InFlags, InAlignment, bInDynamic, nullptr)
	{
	}

	BufferDataType& GetShadowData()
	{
		return reinterpret_cast<BufferDataType&>(*GetShadowDataAddress());
	}

private:

};

class FD3D12ConstantBufferResource : public FD3D12BufferResource
{
public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="InSize"></param>
	/// <param name="bDynamic">Whether buffer data resides on system memory. 
	/// If true, GPU should read fresh data from system memory everytime when it access.
	/// This can be slow. But it's acceptable when it's size is small and the data changes frequently
	/// https://therealmjp.github.io/posts/gpu-memory-pool/
	/// </param>
	FD3D12ConstantBufferResource(const uint64_t InSize, const bool bInDynamic = true)
		: FD3D12BufferResource( // @todo : 
			InSize,
			D3D12_RESOURCE_FLAG_NONE,
			0,
			bInDynamic)
	{
	}

	virtual bool IsConstantBuffer() const
	{
		return true;
	}


protected:

};

template <typename ConstantBufferDataType>
class TD3D12ConstantBufferResource : public FD3D12ConstantBufferResource
{
public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="InSize"></param>
	/// <param name="bDynamic">Whether buffer data resides on system memory. 
	/// If true, GPU should read fresh data from system memory everytime when it access.
	/// This can be slow. But it's acceptable when it's size is small and the data changes frequently
	/// https://therealmjp.github.io/posts/gpu-memory-pool/
	/// </param>
	TD3D12ConstantBufferResource(const bool bInDynamic = true)
		: FD3D12ConstantBufferResource(sizeof(ConstantBufferDataType), bInDynamic)
	{
	}

	ConstantBufferDataType& Data()
	{
		return reinterpret_cast<ConstantBufferDataType&>(*GetShadowDataAddress());
	}

protected:

};

class FD3D12RenderTargetResource : public FD3D12Resource
{
public:

	FD3D12RenderTargetResource(ComPtr<ID3D12Resource> InRenderTargetResource);

	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsTexture() const
	{
		return true;
	}

	virtual void InitResource();

private:

};

class FD3D12DepthStencilTargetResource : public FD3D12Resource
{
public:

	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsTexture() const
	{
		return true;
	}

};