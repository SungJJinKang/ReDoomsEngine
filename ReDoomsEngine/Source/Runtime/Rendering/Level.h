#pragma once
#include "CommonInclude.h"
#include "Mesh.h"

class FLevel
{
public:

	void UploadModel(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePath);

private:

	eastl::vector<eastl::shared_ptr<FMeshModel>> ModelList;

};

