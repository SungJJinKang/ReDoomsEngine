#include "GPUScene.h"

void GPUScene::Init()
{
	GPUSceneBuffer = eastl::make_unique<FD3D12ConstantBufferResource>(GSupportedMaxPrimitiveCount * sizeof(FPrimitiveSceneData), false);
}

void GPUScene::UploadDirtyData(const FRenderObjectList& InRenderObjectList, const eastl::bitvector<>& InDirtyObjectList)
{
	FD3D12ResourceUploadBatcher& ResoruceUploadBatcher =
		EA::StdC::Singleton<FD3D12ResourceAllocator>::GetInstance()->ResourceUploadBatcher;


	FD3D12ResourceUpload ResourceUpload{};
	ResourceUpload.Resource = GPUSceneBuffer->GetResource();
	ResourceUpload.SubresourceContainers

	for (int32 ObjectIndex = 0 ; ObjectIndex < InDirtyObjectList.size() ; ++ObjectIndex)
	{
		if (InDirtyObjectList[ObjectIndex])
		{
			
		}
	}

	eastl::vector<FD3D12ResourceUpload&&> ResourceUploads;
	ResoruceUploadBatcher.AddPendingResourceUploads(ResourceUploads);

}
