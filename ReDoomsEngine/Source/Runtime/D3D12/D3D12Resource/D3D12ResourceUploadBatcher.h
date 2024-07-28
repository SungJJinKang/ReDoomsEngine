#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Fence.h"
#include "D3D12CommandContext.h"
#include "DirectXTex.h"

class FD3D12BufferResource;
class FD3D12Texture2DResource;
struct FD3D12UploadBufferBlock;

namespace Assimp {
	class Importer;
}

struct FD3D12SubresourceContainer
{
	virtual ~FD3D12SubresourceContainer() = default;

	uint64 DstOffset = 0;
	D3D12_SUBRESOURCE_DATA SubresourceData;
};

struct FD3D12VertexIndexBufferSubresourceContainer : public FD3D12SubresourceContainer
{
	FD3D12VertexIndexBufferSubresourceContainer() = delete;
	FD3D12VertexIndexBufferSubresourceContainer(const uint8_t* const InData, const size_t InSize, eastl::shared_ptr<Assimp::Importer>& AssimpImporter);
	FD3D12VertexIndexBufferSubresourceContainer(eastl::vector<uint8_t>&& InCopiedData);

	void InitSubresourceData();

	const eastl::vector<uint8_t> ShadowDataStorage;
	const uint8_t* const Data;
	const size_t Size;

	const eastl::shared_ptr<Assimp::Importer> AssimpImporter;
};

struct FD3D12ConstantBufferSubresourceContainer : public FD3D12SubresourceContainer
{
	FD3D12ConstantBufferSubresourceContainer() = delete;
	FD3D12ConstantBufferSubresourceContainer(const uint8_t* const InData, const size_t InSize);
	FD3D12ConstantBufferSubresourceContainer(const eastl::vector<uint8_t>& InCopiedData);
	FD3D12ConstantBufferSubresourceContainer(eastl::vector<uint8_t>&& InCopiedData);

	void InitSubresourceData();

	const eastl::vector<uint8_t> ShadowDataStorage;
	const uint8_t* const Data;
	const size_t Size;
};

struct FD3D12TextureSubresourceContainer : public FD3D12SubresourceContainer
{
	DirectX::ScratchImage ScreatchImage;
};

struct FD3D12ResourceUpload
{
	ID3D12Resource* Resource;
	eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>> SubresourceContainers;

	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersBeforeUpload;
	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersAfterUpload;
};

enum class ED3D12UploadBufferSizeType : uint32_t
{
	Small,
	Medium,
	Large,
	FourK,
	Num
};

struct FD3D12UploadBufferContainer
{
	eastl::unique_ptr<FD3D12BufferResource> UploadBuffer;
	eastl::weak_ptr<FD3D12Fence> Fence;
	uint64_t UploadedFrameIndex;

	bool CanFree() const;
};

class FD3D12ResourceUploadBatcher
{
public:

	void AddPendingResourceUpload(FD3D12ResourceUpload&& InResourceUpload);
	void Flush(FD3D12CommandContext& InCommandContext);
	void FreeUnusedUploadBuffers();

private:

	FD3D12UploadBufferContainer* AllocateUploadBuffer(ID3D12Resource* const InUploadedResource);
	
	static ED3D12UploadBufferSizeType ConvertSizeToUploadBufferSizeType(const uint64_t InSize);
	static uint64_t ConvertUploadBufferSizeTypeToSize(const ED3D12UploadBufferSizeType InUploadBufferSizeType);

	eastl::array<eastl::queue<eastl::unique_ptr<FD3D12UploadBufferContainer>>, static_cast<uint32_t>(ED3D12UploadBufferSizeType::Num)> UploadBufferQueue;
	eastl::vector<FD3D12ResourceUpload> PendingResourceUploadList;
};

