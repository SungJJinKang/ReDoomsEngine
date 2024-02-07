#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"

#include "D3D12RendererStateCallbackInterface.h"
#include "D3D12ResourceUploadBatcher.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12Resource.h"

class FD3D12Resource;
struct FD3D12CommandContext;
class FD3D12Texture2DResource;

class FD3D12ResourceAllocator : public EA::StdC::Singleton<FD3D12ResourceAllocator>, public ID3D12RendererStateCallbackInterface
{
public:

	void Init();

	eastl::shared_ptr<FD3D12Texture2DResource> Allocate(
		FD3D12CommandContext& InCommandContext,
		eastl::vector<FD3D12SubresourceContainer>&& SubresourceDataList,
		const FD3D12Resource::FResourceCreateProperties& InResourceCreateProperties,
		CD3DX12_RESOURCE_DESC InD3DResourceDesc);


	virtual void OnStartFrame();
	virtual void OnEndFrame();

	FD3D12ResourceUploadBatcher ResourceUploadBatcher{};

private:

	FD3D12ResourcePool AllocateNewPool(const D3D12_HEAP_DESC InHeapDesc);

	eastl::array<FD3D12ResourcePool, FD3D12ResourcePool::EResourcePoolType::Num> TexturePoolList;

};

