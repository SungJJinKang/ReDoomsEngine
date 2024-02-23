#include "RenderScene.h"

#include "D3D12CommandContext.h"

uint32_t FRenderScene::AddRenderObject(
	const bool bInVisible, 
	const BoundingBox& InBoundingBox, 
	const Vector4& InPositionBoundingSphereRadius, 
	const Matrix& InModelMatrix, 
	const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViews,
	const D3D12_INDEX_BUFFER_VIEW& IndexBufferView, 
	const FD3D12PSOInitializer::FDrawDesc& InDrawDesc,
	const FMeshDrawArgument& InMeshDrawArgument
)
{
	RenderObjectList.VisibleFlagsList.push_back(bInVisible);
	RenderObjectList.BoundingBoxList.push_back(InBoundingBox);
	RenderObjectList.PositionBoundingSphereRadiusList.push_back(InPositionBoundingSphereRadius);
	RenderObjectList.ModelMatrixList.push_back(InModelMatrix);
	RenderObjectList.VertexBufferViewList.push_back(InVertexBufferViews);
	RenderObjectList.IndexBufferViewList.push_back(IndexBufferView);
	RenderObjectList.DrawDescList.push_back(InDrawDesc);
	RenderObjectList.MeshDrawArgumentList.push_back(InMeshDrawArgument);

	return RenderObjectList.VisibleFlagsList.size() - 1;
}

uint32_t FRenderScene::AddDraw(const EPass InPass, const uint32_t InObjectIndex, const FBoundShaderSet& InBoundShaderSet)
{
	FPass& Pass = PerPassData[static_cast<uint32_t>(InPass)];

	Pass.DrawedObjectIndexList.push_back(InObjectIndex);

	const eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency>& ShaderTemplateListOfRenderObject = RenderObjectList.DrawDescList[InObjectIndex].BoundShaderSet.GetShaderTemplateList();
	const eastl::array<FD3D12ShaderTemplate*, EShaderFrequency::NumShaderFrequency>& ShaderTemplateListOfInput = InBoundShaderSet.GetShaderTemplateList();
	for (uint32_t ShaderFrequencyIndex = 0; ShaderFrequencyIndex < EShaderFrequency::NumShaderFrequency; ++ShaderFrequencyIndex)
	{
		EA_ASSERT(ShaderTemplateListOfRenderObject[ShaderFrequencyIndex] == ShaderTemplateListOfInput[ShaderFrequencyIndex]);
	}
	Pass.BoundShaderSetList.push_back(InBoundShaderSet);


	return Pass.DrawedObjectIndexList.size() - 1;
}

eastl::vector<FMeshDraw> FRenderScene::CreateMeshDrawListForPass(const EPass InPass)
{
	eastl::vector<FMeshDraw> MeshDrawList;

	FPass& Pass = PerPassData[static_cast<uint32_t>(InPass)];

	MeshDrawList.reserve(Pass.DrawedObjectIndexList.size());

	auto ObjectIndexIter = Pass.DrawedObjectIndexList.begin();
	auto BoundShaderSetIter = Pass.BoundShaderSetList.begin();

	FD3D12PSOInitializer PSO;
	PSO.PassDesc = Pass.PassPSODesc;

	// Step 1
	// CPU Culling (https://github.com/SungJJinKang/EveryCulling)

	// Step 2
	// Draw!
	while (ObjectIndexIter != Pass.DrawedObjectIndexList.end())
	{
		const uint32_t ObjectIndex = *ObjectIndexIter;
		FBoundShaderSet& BoundShaderSet = *BoundShaderSetIter;
		const FD3D12PSOInitializer::FDrawDesc& DrawDesc = RenderObjectList.DrawDescList[ObjectIndex];
		PSO.DrawDesc = DrawDesc;
		PSO.DrawDesc.BoundShaderSet = BoundShaderSet;

		MeshDrawList.emplace_back(FMeshDraw{ PSO , RenderObjectList.VertexBufferViewList[ObjectIndex], RenderObjectList.IndexBufferViewList[ObjectIndex], RenderObjectList.MeshDrawArgumentList[ObjectIndex]});
		
		++ObjectIndexIter;
		++BoundShaderSetIter;
	}
	//

	return MeshDrawList;
}

void FMeshDraw::Draw(FD3D12CommandContext& InCommandContext)
{
	InCommandContext.StateCache.SetVertexBufferViewList(VertexBufferViewList);
	InCommandContext.StateCache.SetIndexBufferView(IndexBufferView);

	InCommandContext.StateCache.SetPSO(PSO);

	if (MeshDrawArgument.IndexCountPerInstance > 0)
	{
		InCommandContext.DrawIndexedInstanced(
			MeshDrawArgument.IndexCountPerInstance,
			MeshDrawArgument.InstanceCount,
			MeshDrawArgument.StartIndexLocation,
			MeshDrawArgument.BaseVertexLocation,
			MeshDrawArgument.StartInstanceLocation);
	}
	else
	{
		InCommandContext.DrawInstanced(
			MeshDrawArgument.VertexCountPerInstance,
			MeshDrawArgument.InstanceCount,
			MeshDrawArgument.StartVertexLocation,
			MeshDrawArgument.StartInstanceLocation);
	}
}
