#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "DirectXTex.h"

struct FD3D12CommandContext;
class FD3D12CommandQueue;
class FD3D12Texture2DResource;

class FTextureLoader
{
public:

	static eastl::shared_ptr<FD3D12Texture2DResource> LoadFromFile(FD3D12CommandContext& InCommandContext, 
		const wchar_t* const InRelativePathToAssetFolder,
		const D3D12_RESOURCE_FLAGS InD3DResourceFlags,
		const DirectX::CREATETEX_FLAGS InCreateTexFlag, const eastl::optional<D3D12_RESOURCE_STATES>& InResourceStateAfterUpload
	);


private:
};

