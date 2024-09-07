#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"

#include "Common/RendererStateCallbackInterface.h"
#include "D3D12ResourceUploadBatcher.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12Resource.h"

class FD3D12Resource;
struct FD3D12CommandContext;
class FD3D12Texture2DResource;

class FD3D12ResourceAllocator : public EA::StdC::Singleton<FD3D12ResourceAllocator>, public IRendererStateCallbackInterface
{
public:

	void Init();

	eastl::shared_ptr<FD3D12Texture2DResource> AllocateTexture2D(
		const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties,
		CD3DX12_RESOURCE_DESC InD3DResourceDesc);

	eastl::shared_ptr<FD3D12Texture2DResource> AllocateTexture2D(
		FD3D12CommandContext& InCommandContext,
		eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>>&& SubresourceDataList,
		const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties,
		CD3DX12_RESOURCE_DESC InD3DResourceDesc, const eastl::optional<D3D12_RESOURCE_STATES>& InResourceStateAfterUpload);

	eastl::shared_ptr<FD3D12Texture3DResource> AllocateTexture3D(
		const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties,
		CD3DX12_RESOURCE_DESC InD3DResourceDesc);

	eastl::shared_ptr<FD3D12Texture2DResource> AllocateRenderTarget2D(
		const uint32_t InWidth,
		const uint32_t InHeight,
		const float InClearValue[4],
		const ETextureFormat InTextureFormat = ETextureFormat::SceneColor
	);
	eastl::shared_ptr<FD3D12Texture2DResource> AllocateRenderTargetCube(
		const uint32_t InWidth,
		const uint32_t InHeight,
		const float InClearValue[4],
		const ETextureFormat InTextureFormat = ETextureFormat::SceneColor
	);
	eastl::shared_ptr<FD3D12Texture3DResource> AllocateRenderTarget3D(
		const uint32_t InWidth,
		const uint32_t InHeight,
		const uint32_t InDepth,
		const float InClearValue[4],
		const ETextureFormat InTextureFormat = ETextureFormat::SceneColor
	);

	eastl::shared_ptr<FD3D12Texture2DResource> AllocateDepthStencilTarget(
		const uint32_t InWidth,
		const uint32_t InHeight,
		const float InDepthClearValue = 0.0f,
		const float InStencilClearValue = 0.0f,
		const ETextureFormat InTextureFormat = ETextureFormat::DepthStencil
	);

	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);
	
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticVertexBuffer(
		FD3D12CommandContext& InCommandContext,
		const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes);
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticIndexBuffer(
		FD3D12CommandContext& InCommandContext,
		const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes);

	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticVertexBuffer(
		FD3D12CommandContext& InCommandContext,
		const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes, eastl::shared_ptr<Assimp::Importer> Importer);
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticIndexBuffer(
		FD3D12CommandContext& InCommandContext,
		const uint8_t* const Data, const size_t InSize, const uint32_t InDefaultStrideInBytes, eastl::shared_ptr<Assimp::Importer> Importer);
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticVertexBuffer(
		FD3D12CommandContext& InCommandContext,
		eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes);
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticIndexBuffer(
		FD3D12CommandContext& InCommandContext,
		eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes);

	FD3D12ResourceUploadBatcher ResourceUploadBatcher{};

private:

	eastl::shared_ptr<FD3D12VertexIndexBufferResource> AllocateStaticVertexIndexBuffer(
		FD3D12CommandContext& InCommandContext,
		eastl::unique_ptr<FD3D12SubresourceContainer>&& SubresourceDataList, const uint32_t InDefaultStrideInBytes,
		const bool bVertexBuffer);

	FD3D12ResourcePool AllocateNewPool(const D3D12_HEAP_DESC InHeapDesc);

	eastl::array<FD3D12ResourcePool, FD3D12ResourcePool::EResourcePoolType::Num> TexturePoolList;

};

void FlushResourceUploadBatcher(FD3D12CommandContext& InCommandContext);
