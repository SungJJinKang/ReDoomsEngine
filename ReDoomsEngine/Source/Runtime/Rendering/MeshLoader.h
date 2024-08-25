#pragma once

#include "CommonInclude.h"

#include "D3D12Resource/D3D12Resource.h"
#include "Mesh.h"

class FMeshLoader
{
public:

	static eastl::vector<FMeshModel> LoadFromMeshFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePathToAssetFolder);

private:
};

