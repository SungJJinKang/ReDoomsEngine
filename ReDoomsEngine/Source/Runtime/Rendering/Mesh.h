#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"

#define MAX_NUMBER_OF_VERTEXCOLOR 8
#define MAX_NUMBER_OF_TEXTURECOORDS 5

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

enum class EShadingModel
{
	Unknown,
	Default
};

enum class ETextureMapping 
{
	Unknown,

	UV,

	/** Spherical mapping */
	Sphere,

	/** Cylindrical mapping */
	Cylinder,

	/** Cubic mapping */
	Box,

	/** Planar mapping */
	Plane
};

enum class ETextureOp 
{
	Unknown,

	/** T = T1 * T2 */
	Multiply = 0x0,

	/** T = T1 + T2 */
	Add = 0x1,

	/** T = T1 - T2 */
	Subtract = 0x2,

	/** T = T1 / T2 */
	Divide = 0x3,

	/** T = (T1 + T2) - (T1 * T2) */
	SmoothAdd = 0x4,

	/** T = T1 + (T2-0.5) */
	SignedAdd = 0x5
};

enum class ETextureMapMode 
{
	Unknown,
	
	/** A texture coordinate u|v is translated to u%1|v%1
	 */
	Wrap,

	/** Texture coordinates outside [0...1]
	 *  are clamped to the nearest valid value.
	 */
	Clamp,

	/** If the texture coordinates for a pixel are outside [0...1]
	 *  the texture is not applied to that pixel
	 */
	Decal,

	/** A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and
	 *  1-(u%1)|1-(v%1) otherwise
	 */
	Mirror
};

struct FMaterial
{
	// PBR
	EShadingModel ShadingModel = EShadingModel::Unknown;
	ETextureMapping TextureMapping = ETextureMapping::Unknown;
	int32 UVIndex = -1;
	float Blend = -1.0f;
	ETextureOp TextureOp = ETextureOp::Unknown;
	ETextureMapMode TextureMapMode[2]{ ETextureMapMode::Unknown, ETextureMapMode::Unknown };

	eastl::shared_ptr<FD3D12Texture2DResource> BaseColor;
	eastl::shared_ptr<FD3D12Texture2DResource> Emissive;
	eastl::shared_ptr<FD3D12Texture2DResource> Metalic;
	eastl::shared_ptr<FD3D12Texture2DResource> Roughness;
	eastl::shared_ptr<FD3D12Texture2DResource> AmbientOcclusion;
};

struct FMeshModel
{
	FMesh Mesh;
	FMaterial Material;
};