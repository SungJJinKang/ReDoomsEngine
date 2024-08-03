#pragma once
#include "CommonInclude.h"

struct FRenderObjectList;
class FD3D12ConstantBufferResource;
struct FD3D12CommandContext;

#define PRIMITIVE_SCENEDATA_VARIABLE_NAME "PrimitiveSceneData"


// this shoud match with struct in SceneData.hlsl
struct FPrimitiveSceneData
{
	uint32	Flags;
	uint32	VisibilityFlags;
	Matrix	LocalToWorld;
	Matrix	WorldToLocal;
};

class GPUScene
{
public:

	void Init();
	void UploadDirtyData(FD3D12CommandContext& InCommandContext, FRenderObjectList& InRenderObjectList);
	FD3D12ConstantBufferResource* GetGPUSceneBuffer();
	uint32 GetPrimitiveCount() const;
	uint32 GetSceneBufferSize() const;

private:

	uint32 PrimitiveCount = 0;
	eastl::shared_ptr<FD3D12ConstantBufferResource> GPUSceneBuffer;

};

