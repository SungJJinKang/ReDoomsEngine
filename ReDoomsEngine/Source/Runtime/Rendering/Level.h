#pragma once
#include "CommonInclude.h"
#include "Mesh.h"
#include "MeshLoader.h"

class FLevel
{
public:

	void UploadModel(
		FD3D12CommandContext& InCommandContext,
		const wchar_t* const InRelativePath,
		const FMeshModelCustomData& InMeshModelCustomData = FMeshModelCustomData{},
		const EMeshLoadFlags InMeshLoadFlags = EMeshLoadFlags::MeshLoadFlags_None
	);
	eastl::vector<FMeshModel> ModelList;

private:

};

