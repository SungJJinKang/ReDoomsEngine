#include "Level.h"
#include "MeshLoader.h"

void FLevel::UploadModel(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePath, const EMeshLoadFlags InMeshLoadFlags)
{
	eastl::vector<FMeshModel> MeshModelList = FMeshLoader::LoadFromMeshFile(InCommandContext, InRelativePath, InMeshLoadFlags);

	for(FMeshModel& MeshModel : MeshModelList)
	{
		ModelList.emplace_back(MeshModel);
	}
}

