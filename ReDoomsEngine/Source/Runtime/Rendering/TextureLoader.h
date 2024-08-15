#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "DirectXTex.h"

struct FD3D12CommandContext;
class FD3D12CommandQueue;
class FD3D12Texture2DResource;

struct FTextureLoadInfo
{
	eastl::wstring RelativePathToAssetFolder;
	D3D12_RESOURCE_FLAGS D3DResourceFlags;
	DirectX::CREATETEX_FLAGS CreateTexFlag;
	eastl::optional<D3D12_RESOURCE_STATES> ResourceStateAfterUpload;
};

inline bool operator==(const FTextureLoadInfo& lhs, const FTextureLoadInfo& rhs)
{
    return lhs.RelativePathToAssetFolder == rhs.RelativePathToAssetFolder &&
           lhs.D3DResourceFlags == rhs.D3DResourceFlags &&
           lhs.CreateTexFlag == rhs.CreateTexFlag &&
           lhs.ResourceStateAfterUpload == rhs.ResourceStateAfterUpload;
}

inline bool operator!=(const FTextureLoadInfo& lhs, const FTextureLoadInfo& rhs)
{
    return !(lhs == rhs);
}

namespace eastl
{
	template <> struct hash<FTextureLoadInfo>
	{
		inline size_t operator()(FTextureLoadInfo val) const 
		{
			size_t Hash = CityHash64(reinterpret_cast<const char*>(val.RelativePathToAssetFolder.data()), sizeof(val.RelativePathToAssetFolder.size())) ^
				CITY_HASH64(val.D3DResourceFlags) ^	CITY_HASH64(val.CreateTexFlag);

			if(val.ResourceStateAfterUpload.has_value())
			{
				Hash ^= CITY_HASH64(val.ResourceStateAfterUpload.value());
			}

			return Hash;
		}
	};
}

class FTextureLoader
{
public:

	static eastl::shared_ptr<FD3D12Texture2DResource> LoadFromFile(FD3D12CommandContext& InCommandContext,
		const wchar_t* const InRelativePathToAssetFolder,
		const D3D12_RESOURCE_FLAGS InD3DResourceFlags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
		const DirectX::CREATETEX_FLAGS InCreateTexFlag = DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
		const eastl::optional<D3D12_RESOURCE_STATES>& InResourceStateAfterUpload = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

private:

	static eastl::hash_map<FTextureLoadInfo, eastl::shared_ptr<FD3D12Texture2DResource>> LoadedTextureResourceMap;
};

