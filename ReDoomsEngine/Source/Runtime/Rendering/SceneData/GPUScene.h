#pragma once
#include "CommonInclude.h"

#include "D3D12Resource/D3D12Resource.h"

struct FPrimitiveList;
class FD3D12ConstantBufferResource;
struct FD3D12CommandContext;

#define PRIMITIVE_SCENEDATA_VARIABLE_NAME "GPrimitiveSceneData"


// this shoud match with struct in SceneData.hlsl
struct FPrimitiveSceneData
{
	uint32	Flags;
	uint32	VisibilityFlags;
	uint32	Padding1;
	uint32	Padding2;
	Matrix	LocalToWorld;
	Matrix	WorldToLocal;
	Vector4 WorldAABBCenterAndDrawDistance;
	Vector4 WorldAABBExtent;
};

static_assert(sizeof(FPrimitiveSceneData) % 16 == 0);

class GPUScene
{
public:

	void Init();
	void UploadDirtyData(FD3D12CommandContext& InCommandContext, FPrimitiveList& InPrimitiveList);
	FD3D12ConstantBufferResource* GetGPUSceneBuffer();
	FD3D12ConstantBufferResource* GetPrimitiveIDBuffer();
	uint32 GetPrimitiveCount() const;
	uint32 GetSceneBufferSize() const;

private:

	uint32 PrimitiveCount = 0;

	eastl::shared_ptr<FD3D12ConstantBufferResource> PrimitiveIDBuffer;
	eastl::shared_ptr<FD3D12ConstantBufferResource> GPUSceneBuffer;

};

