#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource.h"

#define MAX_NUMBER_OF_TEXTURECOORDS 5

struct FMesh
{
	inline static const D3D12_INPUT_ELEMENT_DESC InputElementDescs[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 7, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32_FLOAT, 8, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	eastl::shared_ptr<FD3D12VertexIndexBufferResource> PositionBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> NormalBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> TangentBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> BiTangentBuffer;
	eastl::array<eastl::shared_ptr<FD3D12VertexIndexBufferResource>, MAX_NUMBER_OF_TEXTURECOORDS> TexCoordBuffers;

	uint32_t IndexCount;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> IndexBuffer;

	uint32_t MaterialIndex;

	eastl::vector<D3D12_VERTEX_BUFFER_VIEW> CreateVertexBufferViewList() const;
	D3D12_INDEX_BUFFER_VIEW CreateIndexBufferView() const;
};

struct FMeshMaterial
{
	eastl::shared_ptr<FD3D12Texture2DResource> DiffuseTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> EmissiveTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ShininessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> MetalnessTexture;

};

struct F3DModel
{
	eastl::vector<FMesh> MeshList;
	eastl::vector<FMeshMaterial> Material;
};