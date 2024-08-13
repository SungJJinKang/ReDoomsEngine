#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"

#define MAX_NUMBER_OF_VERTEXCOLOR 8
#define MAX_NUMBER_OF_TEXTURECOORDS 1

#define PRIMITIVE_ID_INPUT_ELEMENT_SLOT_INDEX 5

struct FMesh
{
	inline static const D3D12_INPUT_ELEMENT_DESC InputElementDescs[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VERTEXCOLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // @TODO : Support multiple uvs
// 		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 7, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32_FLOAT, 8, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "PRIMITIVEID", 0, DXGI_FORMAT_R32_UINT, PRIMITIVE_ID_INPUT_ELEMENT_SLOT_INDEX, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};
	static_assert(ARRAY_LENGTH(FMesh::InputElementDescs) <= MAX_BOUND_VERTEX_BUFFER_VIEW);
	eastl::wstring MeshName;

	eastl::shared_ptr<FD3D12VertexIndexBufferResource> PositionBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> NormalBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> TangentBuffer;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> BiTangentBuffer;
	eastl::array<eastl::shared_ptr<FD3D12VertexIndexBufferResource>, MAX_NUMBER_OF_VERTEXCOLOR> VertexColorBuffer;
	eastl::array<eastl::shared_ptr<FD3D12VertexIndexBufferResource>, MAX_NUMBER_OF_TEXTURECOORDS> TexCoordBuffers;

	uint32_t IndexCount;
	eastl::shared_ptr<FD3D12VertexIndexBufferResource> IndexBuffer;

	BoundingBox AABB;

	uint32_t MaterialIndex;

	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
};

struct FMeshMaterial
{
	// Non PBR
	eastl::shared_ptr<FD3D12Texture2DResource> DiffuseTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> SpecularTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> AmbientTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> EmissiveTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> HeightTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ShinessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> OpacityTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> DisplacementTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> LightmapTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ReflectionTexture;

	// PBR
	eastl::shared_ptr<FD3D12Texture2DResource> BaseColorTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> EmissionColorTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> MetalnessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> DiffuseRoughnessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> AmbientOcclusionTexture;

	eastl::shared_ptr<FD3D12Texture2DResource> SheenTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ClearCoatTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> TransmissionTexture;
};

struct F3DModel
{
	eastl::vector<FMesh> MeshList;
	eastl::vector<FMeshMaterial> Material;
};