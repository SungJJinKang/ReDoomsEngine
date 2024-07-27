
struct FPrimitiveSceneData
{
	uint		Flags;
	uint		VisibilityFlags;
	float4x4	LocalToWorld;
	float4x4	WorldToLocal;
};

StructuredBuffer<float4> PrimitiveSceneData;

FPrimitiveSceneData GetPrimitiveIdSceneData(uint PrimitiveId)
{
	FPrimitiveSceneData SceneData = (FPrimitiveSceneData)0;
	return SceneData;
}