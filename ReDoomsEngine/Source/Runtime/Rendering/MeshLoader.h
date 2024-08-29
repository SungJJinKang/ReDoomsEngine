#pragma once

#include "CommonInclude.h"

#include "D3D12Resource/D3D12Resource.h"
#include "Mesh.h"

enum EMeshLoadFlags : uint32
{
	MeshLoadFlags_None = 0,
	DontFlipUVs = 1 << 0,
	SubstractOneFromV = 1 << 1
};

class FMeshLoader
{
public:

	static eastl::vector<FMeshModel> LoadFromMeshFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePathToAssetFolder, const EMeshLoadFlags InMeshLoadFlags = EMeshLoadFlags::MeshLoadFlags_None);

private:
};

