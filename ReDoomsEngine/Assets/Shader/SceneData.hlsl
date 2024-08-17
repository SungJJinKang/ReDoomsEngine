#pragma once

struct FPrimitiveSceneData
{
	uint		Flags;
	uint		VisibilityFlags;
	uint		Padding1;
	uint		Padding2;
	float4x4	LocalToWorld;
	float4x4	WorldToLocal;
	float4		AABBCenterAndDrawDistance;
	float4		AABBExtent;
};

#define PRIMITIVE_SCENEDATA_STRIDE 11

StructuredBuffer<float4> GPrimitiveSceneData;

float4 LoadPrimitivePrimitiveSceneDataElement(uint PrimitiveId, uint ItemIndex)
{
	uint PrimitiveOffset = PrimitiveId * PRIMITIVE_SCENEDATA_STRIDE;

	uint TargetElementOffset = PrimitiveOffset + ItemIndex;
	return GPrimitiveSceneData[TargetElementOffset];
}

// @TODO : Support instance id
FPrimitiveSceneData GetPrimitiveIdSceneData(uint PrimitiveId)
{
	FPrimitiveSceneData SceneData = (FPrimitiveSceneData)0;

	SceneData.Flags = asuint(LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 0).x);
	SceneData.VisibilityFlags = asuint(LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 0).y);
		
	SceneData.LocalToWorld = float4x4(
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 1),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 2),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 3),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 4)
	);

	SceneData.WorldToLocal = float4x4(
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 5),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 6),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 7),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 8)
	);

	SceneData.AABBCenterAndDrawDistance = LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 9);
	SceneData.AABBExtent = LoadPrimitivePrimitiveSceneDataElement(PrimitiveId, 10);

	return SceneData;
}