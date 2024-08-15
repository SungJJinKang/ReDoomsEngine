#include "DummyTexture.h"
#include "RenderingInclude.h"

eastl::shared_ptr<FD3D12Texture2DResource> DummyBlackTexture{};
eastl::shared_ptr<FD3D12Texture2DResource> DummyWhiteTexture{};

float DataBlack[4]{ 0.0f,0.0f,0.0f,0.0f };
float DataWhite[4]{ 1.0f,1.0f,1.0f,1.0f };

void InitDummyTexture(FD3D12CommandContext& InCommandContext)
{
	CD3DX12_RESOURCE_DESC ResourceDesc;

	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Width = 1;
		desc.Height = 1;
		desc.MipLevels = 1;
		desc.DepthOrArraySize = 1;
		desc.Format = static_cast<DXGI_FORMAT>(ETextureFormat::SceneColor);
		desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.SampleDesc.Count = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc = CD3DX12_RESOURCE_DESC{ desc };
	}

	FD3D12Resource::FResourceCreateProperties ResourceCreateProperties{};
	ResourceCreateProperties.HeapProperties = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT };
	ResourceCreateProperties.HeapFlags = D3D12_HEAP_FLAG_NONE;
	ResourceCreateProperties.InitialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;

	{
		eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>> SubresourceDataList{};
		eastl::unique_ptr<FD3D12TextureSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12TextureSubresourceContainer>();

		SubresourceContainer->SubresourceData.pData = &DataBlack;
		SubresourceContainer->SubresourceData.RowPitch = 1;
		SubresourceContainer->SubresourceData.SlicePitch = 1;

		SubresourceDataList.emplace_back(eastl::move(SubresourceContainer));

		DummyBlackTexture = FD3D12ResourceAllocator::GetInstance()->AllocateTexture2D(InCommandContext, eastl::move(SubresourceDataList), ResourceCreateProperties, ResourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		DummyBlackTexture->SetDebugNameToResource(EA_WCHAR("DummyBlackTexture"));
	}

	{
		eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>> SubresourceDataList{};
		eastl::unique_ptr<FD3D12TextureSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12TextureSubresourceContainer>();

		SubresourceContainer->SubresourceData.pData = &DataWhite;
		SubresourceContainer->SubresourceData.RowPitch = 1;
		SubresourceContainer->SubresourceData.SlicePitch = 1;

		SubresourceDataList.emplace_back(eastl::move(SubresourceContainer));

		DummyWhiteTexture = FD3D12ResourceAllocator::GetInstance()->AllocateTexture2D(InCommandContext, eastl::move(SubresourceDataList), ResourceCreateProperties, ResourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		DummyWhiteTexture->SetDebugNameToResource(EA_WCHAR("DummyWhiteTexture"));
	}
}
