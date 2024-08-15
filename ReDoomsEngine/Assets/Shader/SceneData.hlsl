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

#define PRIMITIVE_SCENEDATA_STRIDE 9

StructuredBuffer<float4> GPrimitiveSceneData;

float4 LoadPrimitivePrimitiveSceneDataElement(uint PrimitiveIndex, uint ItemIndex)
{
	uint TargetElementIdx = PrimitiveIndex + ItemIndex;
	return GPrimitiveSceneData[TargetElementIdx];
}

// @TODO : Support instance id
FPrimitiveSceneData GetPrimitiveIdSceneData(uint PrimitiveId)
{
	FPrimitiveSceneData SceneData = (FPrimitiveSceneData)0;

	uint PrimitiveIndex = PrimitiveId * PRIMITIVE_SCENEDATA_STRIDE;

	SceneData.Flags = asuint(LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 0).x);
	SceneData.VisibilityFlags = asuint(LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 0).y);
		
	SceneData.LocalToWorld = float4x4(
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 1),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 2),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 3),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 4)
	);

	SceneData.WorldToLocal = float4x4(
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 5),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 6),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 7),
		LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 8)
	);

	SceneData.AABBCenterAndDrawDistance = LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 9)
	SceneData.AABBExtent = LoadPrimitivePrimitiveSceneDataElement(PrimitiveIndex, 10)

	return SceneData;
}