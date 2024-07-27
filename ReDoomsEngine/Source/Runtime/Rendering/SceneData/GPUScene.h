#pragma once
#include "CommonInclude.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"

struct FRenderObjectList;

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
	void UploadDirtyData(FD3D12CommandContext& InCommandContext, const FRenderObjectList& InRenderObjectList, const eastl::bitvector<>& InDirtyObjectList);

private:

	eastl::unique_ptr<FD3D12ConstantBufferResource> GPUSceneBuffer;

};

