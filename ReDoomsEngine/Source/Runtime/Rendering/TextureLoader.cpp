#include "TextureLoader.h"
#include "AssetManager.h"
#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"
#include "D3D12Resource/D3D12Resource.h"

eastl::shared_ptr<FD3D12Texture2DResource> FTextureLoader::LoadFromDDSFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePathToAssetFolder, const D3D12_RESOURCE_FLAGS InD3DResourceFlags, const DirectX::CREATETEX_FLAGS InCreateTexFlag)
{
	EA_ASSERT(!(InD3DResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET));
	EA_ASSERT(!(InD3DResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));

	eastl::shared_ptr<FD3D12Texture2DResource> D3D12TextureResource{};

	const eastl::wstring AbsoluePath = FAssetManager::MakeAbsolutePathFromAssetFolder(InRelativePathToAssetFolder);

	DirectX::TexMetadata Metadata{};
	DirectX::ScratchImage ScreatchImage{};

	HRESULT HR = DirectX::LoadFromDDSFile(AbsoluePath.data(), DDS_FLAGS::DDS_FLAGS_NONE, &Metadata, ScreatchImage);
	if (!FAILED(HR))
	{
		// Get the first image from the scratch image
		const DirectX::Image* const DXImage = ScreatchImage.GetImages();

		D3D12_RESOURCE_DESC ResourceDesc;

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
		

		ComPtr<ID3D12Heap> TextureHeap; // Heap that will hold the texture

		D3D12_HEAP_DESC HeapDesc = {};
		HeapDesc.SizeInBytes = 1024 * 1024 * 64; // Adjust the size as needed
		HeapDesc.Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HeapDesc.Alignment = 0;
		HeapDesc.Flags = D3D12_HEAP_FLAG_NONE;
		VERIFYD3D12RESULT(GetD3D12Device()->CreateHeap(&HeapDesc, IID_PPV_ARGS(&TextureHeap)));

		D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		
		ComPtr<ID3D12Resource> TextureResource;
		VERIFYD3D12RESULT(GetD3D12Device()->CreatePlacedResource(
			TextureHeap.Get(),
			0, // Offset in bytes from the beginning of the heap
			&ResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&TextureResource)
		));

		// Upload the texture data to the GPU
		const UINT subresourceIndex = 0;
		const UINT64 UploadBufferSize = GetRequiredIntermediateSize(TextureResource.Get(), subresourceIndex, 1);

		ComPtr<ID3D12Resource> UploadHeap;
		CD3DX12_HEAP_PROPERTIES UploadHeapProperties = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD };
		CD3DX12_RESOURCE_DESC UploadHeapDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);
		VERIFYD3D12RESULT(GetD3D12Device()->CreateCommittedResource(
			&UploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&UploadHeapDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&UploadHeap)
		));

		D3D12_SUBRESOURCE_DATA SubresourceData = {};
		SubresourceData.pData = DXImage->pixels;
		SubresourceData.RowPitch = static_cast<LONG_PTR>(DXImage->rowPitch);
		SubresourceData.SlicePitch = static_cast<LONG_PTR>(DXImage->slicePitch);

		const UINT64 Result = UpdateSubresources(InCommandContext.CommandList->GetD3DCommandList(), TextureResource.Get(), UploadHeap.Get(), 0, 0, 1, &SubresourceData);
		EA_ASSERT(Result > 0);

		FD3D12Resource::FResourceCreateProperties ResourceCreateProperties{};
		ResourceCreateProperties.HeapProperties = CD3DX12_HEAP_PROPERTIES{ HeapDesc.Properties };
		ResourceCreateProperties.HeapFlags = D3D12_HEAP_FLAG_NONE;
		ResourceCreateProperties.InitialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
		D3D12TextureResource = eastl::make_shared<FD3D12Texture2DResource>(TextureResource, ResourceCreateProperties, CD3DX12_RESOURCE_DESC{ ResourceDesc });
	}
	else
	{
		RD_LOG(ELogVerbosity::Fatal, EA_WCHAR("\"DirectX::LoadFromDDSFile\" fail (Error Code : %ld)"), HR);
	}

	return D3D12TextureResource;
}
