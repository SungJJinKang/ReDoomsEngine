#include "D3D12Mesh.h"

eastl::vector<D3D12_VERTEX_BUFFER_VIEW> FMesh::CreateVertexBufferViewList() const
{
	eastl::vector<D3D12_VERTEX_BUFFER_VIEW> VertexBufferViewList{};

	VertexBufferViewList.emplace_back(PositionBuffer->GetVertexBufferView());
	VertexBufferViewList.emplace_back(NormalBuffer->GetVertexBufferView());
	VertexBufferViewList.emplace_back(TangentBuffer->GetVertexBufferView());
	VertexBufferViewList.emplace_back(BiTangentBuffer->GetVertexBufferView());

	for (uint32_t UVIndex = 0; UVIndex < MAX_NUMBER_OF_TEXTURECOORDS ; ++UVIndex)
	{
		if (TexCoordBuffers[UVIndex])
		{
			VertexBufferViewList.emplace_back(TexCoordBuffers[UVIndex]->GetVertexBufferView());
		}
	}

	return VertexBufferViewList;
}

D3D12_INDEX_BUFFER_VIEW FMesh::CreateIndexBufferView() const
{
	return IndexBuffer->GetIndexBufferView();
}
