#include "TextureLoader.h"

#include <filesystem>

#include "AssetManager.h"
#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"
#include "D3D12Resource/D3D12Resource.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "Utils/ConsoleVariable.h"

static TConsoleVariable<bool> GSupportTextureMips{ "r.SupportTextureMips", false };

eastl::hash_map<FTextureLoadInfo, eastl::shared_ptr<FD3D12Texture2DResource>> FTextureLoader::LoadedTextureResourceMap{};

eastl::shared_ptr<FD3D12Texture2DResource> FTextureLoader::LoadFromFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePathToAssetFolder, 
	const D3D12_RESOURCE_FLAGS InD3DResourceFlags, const DirectX::CREATETEX_FLAGS InCreateTexFlag, const eastl::optional<D3D12_RESOURCE_STATES>& InResourceStateAfterUpload)
{
	EA_ASSERT(!(InD3DResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET));
	EA_ASSERT(!(InD3DResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));

	eastl::shared_ptr<FD3D12Texture2DResource> D3D12TextureResource{};

	FTextureLoadInfo TextureLoadInfo{};
	TextureLoadInfo.RelativePathToAssetFolder = InRelativePathToAssetFolder;
	TextureLoadInfo.D3DResourceFlags = InD3DResourceFlags;
	TextureLoadInfo.CreateTexFlag = InCreateTexFlag;
	TextureLoadInfo.ResourceStateAfterUpload = InResourceStateAfterUpload;

	auto LoadedTextureResourceIter = LoadedTextureResourceMap.find(TextureLoadInfo);
	if(LoadedTextureResourceIter != LoadedTextureResourceMap.end())
	{
		D3D12TextureResource = LoadedTextureResourceIter->second;
	}
	else
	{
		std::filesystem::path AbsoluePath{ FAssetManager::MakeAbsolutePathFromAssetFolder(InRelativePathToAssetFolder).c_str() };

		DirectX::TexMetadata Metadata{};
		DirectX::ScratchImage ScreatchImage{};

		//DirectX::LoadFromWICFile()
		HRESULT HR;
		if (AbsoluePath.extension().string() == ".dds")
		{
			HR = DirectX::LoadFromDDSFile(AbsoluePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, &Metadata, ScreatchImage);
		}
		else
		{
			HR = DirectX::LoadFromWICFile(AbsoluePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &Metadata, ScreatchImage);
		}

		if (!FAILED(HR))
		{
			// Get the first image from the scratch image
			const DirectX::Image* const DXImage = ScreatchImage.GetImages();

			CD3DX12_RESOURCE_DESC ResourceDesc;

			if (!GSupportTextureMips)
			{
				Metadata.mipLevels = 1;
			}

			{
				DXGI_FORMAT DXGIFormat = Metadata.format;
				if (InCreateTexFlag & CREATETEX_FORCE_SRGB)
				{
					DXGIFormat = MakeSRGB(DXGIFormat);
				}
				else if (InCreateTexFlag & CREATETEX_IGNORE_SRGB)
				{
					DXGIFormat = MakeLinear(DXGIFormat);
				}

				D3D12_RESOURCE_DESC desc = {};
				desc.Width = static_cast<UINT>(Metadata.width);
				desc.Height = static_cast<UINT>(Metadata.height);

				if (DXGIFormat == DXGI_FORMAT::DXGI_FORMAT_BC5_UNORM)
				{
					desc.Width = Align(desc.Width, 4);
					desc.Height = Align(desc.Height, 4);
				}

				desc.MipLevels = static_cast<UINT16>(Metadata.mipLevels);
				desc.DepthOrArraySize = (Metadata.dimension == TEX_DIMENSION_TEXTURE3D)
					? static_cast<UINT16>(Metadata.depth)
					: static_cast<UINT16>(Metadata.arraySize);
				desc.Format = DXGIFormat;
				desc.Flags = InD3DResourceFlags;
				desc.SampleDesc.Count = 1;
				desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(Metadata.dimension);
				ResourceDesc = CD3DX12_RESOURCE_DESC{ desc };
			}

			eastl::vector<eastl::unique_ptr<FD3D12SubresourceContainer>> SubresourceDataList{};

			eastl::shared_ptr<DirectX::ScratchImage> ScratchImageSharedPtr
				= eastl::make_shared<DirectX::ScratchImage>(eastl::move(ScreatchImage));
			
			for(int32 MipIndex = 0 ; MipIndex < Metadata.mipLevels ; ++MipIndex)
			{
				eastl::unique_ptr<FD3D12TextureSubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12TextureSubresourceContainer>();
				SubresourceContainer->ScreatchImage = ScratchImageSharedPtr;
				SubresourceContainer->SubresourceData.pData = DXImage[MipIndex].pixels;
				SubresourceContainer->SubresourceData.RowPitch = static_cast<LONG_PTR>(DXImage[MipIndex].rowPitch);
				SubresourceContainer->SubresourceData.SlicePitch = static_cast<LONG_PTR>(DXImage[MipIndex].slicePitch);
				SubresourceContainer->MipIndex = MipIndex;

				SubresourceDataList.emplace_back(eastl::move(SubresourceContainer));
			}

			FD3D12Resource::FResourceCreateProperties ResourceCreateProperties{};
			ResourceCreateProperties.HeapProperties = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT };
			ResourceCreateProperties.HeapFlags = D3D12_HEAP_FLAG_NONE;
			ResourceCreateProperties.InitialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;

			D3D12TextureResource = FD3D12ResourceAllocator::GetInstance()->AllocateTexture2D(InCommandContext, eastl::move(SubresourceDataList), ResourceCreateProperties, ResourceDesc, InResourceStateAfterUpload);
			D3D12TextureResource->SetDebugNameToResource(InRelativePathToAssetFolder);

			LoadedTextureResourceMap.emplace(TextureLoadInfo, D3D12TextureResource);
		}
		else
		{
			RD_LOG(ELogVerbosity::Fatal, EA_WCHAR("\"DirectX::LoadFromDDSFile\" fail (Error Code : %ld)"), HR);
		}
	}

	return D3D12TextureResource;
}