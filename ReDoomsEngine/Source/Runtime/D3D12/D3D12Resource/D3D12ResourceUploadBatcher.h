#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Fence.h"
#include "D3D12CommandContext.h"

class FD3D12BufferResource;
class FD3D12Texture2DResource;
struct FD3D12UploadBufferBlock;

struct FResourceUpload
{
	eastl::shared_ptr<FD3D12Texture2DResource> Resource;
	D3D12_SUBRESOURCE_DATA SubresourceData;

	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersBeforeUpload;
	eastl::vector<CD3DX12_RESOURCE_BARRIER> ResourceBarriersAfterUpload;
};

enum EUploadBufferSizeType
{
	Small,
	Medium,
	Large,
	VeryLarge,
	Num
};

class FD3D12ResourceUploadBatcher
{
public:

	void AddPendingResourceUpload(const FResourceUpload& InResourceUpload);
	void Flush(FD3D12CommandContext& InCommandContext);

private:

	FD3D12BufferResource* AllocateUploadBuffer(const FD3D12Resource* const InResource);
	static EUploadBufferSizeType ConvertSizeToUploadBufferSizeType(const uint64_t InSize);
	static uint64_t ConvertUploadBufferSizeTypeToSize(const EUploadBufferSizeType InUploadBufferSizeType);

	eastl::array<eastl::queue<eastl::unique_ptr<FD3D12BufferResource>>, EUploadBufferSizeType::Num> UploadBufferQueue;
	eastl::vector<FResourceUpload> PendingResourceUploadList;
};

