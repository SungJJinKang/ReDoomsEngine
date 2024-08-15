#include "Level.h"
#include "MeshLoader.h"

void FLevel::UploadModel(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePath)
{
	eastl::vector<eastl::shared_ptr<FMeshModel>> MeshModelList = FMeshLoader::LoadFromMeshFile(InCommandContext, InRelativePath);

	for(eastl::shared_ptr<FMeshModel>& MeshModel : MeshModelList)
	{
		EA_ASSERT(MeshModel);
		ModelList.emplace_back(MeshModel);
	}
}

