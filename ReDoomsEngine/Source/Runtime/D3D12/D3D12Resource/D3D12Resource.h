#pragma once
#include "D3D12Include.h"

#include "D3D12View.h"
#include "D3D12ConstantBufferRingBuffer.h"
#include "D3D12ResourcePool.h"
#include "D3D12Fence.h"
#include "D3D12ViewDesc.h"

class FD3D12Resource : public eastl::enable_shared_from_this<FD3D12Resource>
{
public:

	virtual ~FD3D12Resource() = default;

	struct FResourceCreateProperties
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties{};
		D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;
		D3D12_RESOURCE_STATES InitialResourceStates = D3D12_RESOURCE_STATE_COMMON;
		eastl::optional<D3D12_CLEAR_VALUE> ClearValue;
	};

	inline bool IsInit() const 
	{
		return bInit;
	}
	virtual void InitResource();
	void DeferredRelease();
	virtual void CreateD3D12Resource();
	virtual bool IsCreateD3D12ResourceOnInitResource() const
	{
		return true;
	}
	virtual void ReleaseResource();
	#if D3D_NAME_OBJECT
	void SetDebugNameToResource(const wchar_t* const InDebugName);
	#else
	void SetDebugNameToResource(const wchar_t* const InDebugName) {}
	#endif
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
	inline bool HasClearValue() const
	{
		return ResourceCreateProperties.ClearValue.has_value();
	}
	inline D3D12_CLEAR_VALUE GetClearValue() const
	{
		return ResourceCreateProperties.ClearValue.value();
	}

	inline ID3D12Resource* GetResource(const uint32_t Index = 0) const 
	{
		return Resources[Index].Get();
	}
	virtual D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const;

	virtual bool IsBuffer() const = 0;
	virtual bool IsTexture() const = 0;

	FD3D12ConstantBufferView* GetCBV();
	FD3D12ShaderResourceView* GetSRV(const FD3D12SRVDesc InD3D12SRVDesc);
	FD3D12UnorderedAccessView* GetUAV();
	FD3D12RenderTargetView* GetRTV();
	FD3D12DepthStencilView* GetDSV();

	FD3D12Fence Fence;

protected:

	FD3D12Resource() = delete;
	FD3D12Resource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12Resource(ComPtr<ID3D12Resource>& InResource);
	FD3D12Resource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);

	bool bInit;

	// Why does require multiple d3d resource? : To prevent previous frame's static(non-dynamic) constant buffer data from being overwritten 
	eastl::array<ComPtr<ID3D12Resource>, GNumBackBufferCount> Resources{};
	FResourceCreateProperties ResourceCreateProperties;
	CD3DX12_RESOURCE_DESC Desc;

private:

	eastl::shared_ptr<FD3D12ConstantBufferView> DefaultCBV;
	eastl::shared_ptr<FD3D12ShaderResourceView> DefaultSRV;
	eastl::hash_map<FD3D12SRVDesc, eastl::shared_ptr<FD3D12ShaderResourceView>> CachedSRVMap;
	eastl::shared_ptr<FD3D12UnorderedAccessView> DefaultUAV;
	eastl::shared_ptr<FD3D12RenderTargetView> DefaultRTV;
	eastl::shared_ptr<FD3D12DepthStencilView> DefaultDSV;
};

class FD3D12TextureResource : public FD3D12Resource
{

protected:

	FD3D12TextureResource(ComPtr<ID3D12Resource>& InResource, const FD3D12ResourcePoolBlock& InResourcePoolBlock, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12TextureResource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12TextureResource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);

	// This constructor is for swap chain
	FD3D12TextureResource(ComPtr<ID3D12Resource> InRenderTargetResource);

	virtual ~FD3D12TextureResource();

	virtual bool IsBuffer() const
	{
		return false;
	}
	virtual bool IsTexture() const
	{
		return true;
	}
	virtual bool Is2DTexture() const = 0;

	eastl::optional<FD3D12ResourcePoolBlock> ResourcePoolBlock;
};

class FD3D12Texture2DResource : public FD3D12TextureResource
{
public:
	FD3D12Texture2DResource(ComPtr<ID3D12Resource>& InResource, const FD3D12ResourcePoolBlock& InResourcePoolBlock, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12Texture2DResource(ComPtr<ID3D12Resource>& InResource, const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);
	FD3D12Texture2DResource(const FResourceCreateProperties& InResourceCreateProperties, const CD3DX12_RESOURCE_DESC& InDesc);

	// This constructor is for swap chain
	FD3D12Texture2DResource(ComPtr<ID3D12Resource> InRenderTargetResource, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat, const uint32_t InSampleCount, const uint32_t InSampleQuality);

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

	virtual bool Is2DTexture() const
	{
		return true;
	}
};


class FD3D12BufferResource : public FD3D12Resource
{
public:

	FD3D12BufferResource(const uint64_t InSize, const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment, const bool bInDynamic, const D3D12_RESOURCE_STATES InInitialResourceState, uint8_t* const InShadowDataAddress = nullptr,
		const uint32_t InShadowDataSize = 0, const bool bNeverCreateShadowData = false);
	
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
	virtual void CreateD3D12Resource();
	virtual void ReleaseResource();

	inline bool IsDynamicBuffer() const 
	{
		return bDynamic;
	}

	uint64_t GetBufferSize() const
	{
		return Desc.Width;
	}

	/// <summary>
	/// Mapped address is on write-combined page so it's recommended to copy data using memcpy style copy and not to read it
	/// </summary>
	/// <returns></returns>
	virtual uint8_t* GetMappedAddress() const;
	uint8_t* Map();
	void Unmap();

	uint8_t* GetShadowDataAddress();
	uint32_t GetShadowDataSize() const;
	void FlushShadowData();

	inline bool IsShadowDataDirty() const
	{
		return bIsShadowDataDirtyPerFrame[GCurrentBackbufferIndex];
	}
	inline bool IsShadowDataDirty(const uint32_t InFrameIndex) const
	{
		return bIsShadowDataDirtyPerFrame[InFrameIndex];
	}

protected:

	FResourceCreateProperties MakeResourceCreateProperties(const bool bDynamic, const D3D12_RESOURCE_STATES InInitialResourceState) const;

	bool bDynamic;
	uint8_t* MappedAddress = nullptr;

	bool bShadowDataCreatedFromThisInstance;
	uint8_t* ShadowDataAddress;
	uint32_t ShadowDataSize;

	// if dynamic buffer, first element may be used.
	eastl::array<bool, GNumBackBufferCount> bIsShadowDataDirtyPerFrame;

	eastl::vector<uint8_t> ShadowData;
};

class FD3D12VertexIndexBufferResource : public FD3D12BufferResource
{
public:

	FD3D12VertexIndexBufferResource(const uint64_t InSize, const uint32_t InDefaultStrideInBytes, const bool bInVertexBuffer, const bool bInDynamic = false);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(const uint32_t InStrideInBytes) const;
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(const uint64_t InBaseOffsetInBytes, const uint32_t InSizeInBytes, const uint32_t InStrideInBytes) const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(const uint64_t InBaseOffsetInBytes, const DXGI_FORMAT InFormat, const uint32_t InSizeInBytes) const;

private:

	uint32_t DefaultStrideInBytes;
};

// template <typename BufferDataType>
// class TD3D12BufferResource : public FD3D12BufferResource
// {
// public:
// 	
// 	TD3D12BufferResource(const D3D12_RESOURCE_FLAGS InFlags, const uint64_t InAlignment = 0, const bool bInDynamic = false, uint8_t* const InShadowDataAddress = nullptr, const uint32_t InShadowDataSize = 0)
// 		: FD3D12BufferResource(sizeof(BufferDataType), InFlags, InAlignment, bInDynamic, InShadowDataAddress, InShadowDataSize)
// 	{
// 	}
// 
// 	BufferDataType& GetShadowData()
// 	{
// 		return reinterpret_cast<BufferDataType&>(*GetShadowDataAddress());
// 	}
// 
// private:
// 
// };

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
	FD3D12ConstantBufferResource(const uint64_t InSize, const bool bInDynamic = true, uint8_t* const InShadowDataAddress = nullptr, const uint32_t InShadowDataSize = 0, const bool bNeverCreateShadowData = false, const bool bInNeedVersioning = true)
		: FD3D12BufferResource( // @todo : 
			Align(InSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT),
			D3D12_RESOURCE_FLAG_NONE,
			0,
			bInDynamic,
			bInDynamic ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			InShadowDataAddress,
			InShadowDataSize,
			bNeverCreateShadowData),
		bNeedVersioning(bInNeedVersioning),
		ConstantBufferRingBufferBlock()
	{
	}

	virtual void InitResource();
	virtual void CreateD3D12Resource();
	virtual bool IsCreateD3D12ResourceOnInitResource() const
	{
		return false;
	}
	void MakeDirty();
	void Versioning();

	virtual bool IsConstantBuffer() const
	{
		return true;
	}
	virtual D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const;

protected:

	const bool bNeedVersioning;
	FD3D12ConstantBufferBlock ConstantBufferRingBufferBlock;
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
	TD3D12ConstantBufferResource(const bool bInDynamic = true, uint8_t* const InShadowDataAddress = nullptr)
		: FD3D12ConstantBufferResource(sizeof(ConstantBufferDataType), bInDynamic, InShadowDataAddress)
	{
	}

	ConstantBufferDataType& Data()
	{
		return reinterpret_cast<ConstantBufferDataType&>(*GetShadowDataAddress());
	}

protected:

};