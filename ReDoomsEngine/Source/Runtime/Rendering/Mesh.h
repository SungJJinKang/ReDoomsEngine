#pragma once

#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Resource/D3D12Resource.h"
#include "Math/Transform.h"

#define MAX_NUMBER_OF_VERTEXCOLOR 8
#define MAX_NUMBER_OF_TEXTURECOORDS 1

struct FMesh
{
	inline static const D3D12_INPUT_ELEMENT_DESC InputElementDescs[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VERTEXCOLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD1", 1, DXGI_FORMAT_R32G32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // @TODO : Support multiple uvs
		//{ "TEXCOORD2", 2, DXGI_FORMAT_R32G32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD3", 3, DXGI_FORMAT_R32G32_FLOAT, 7, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
 		//{ "TEXCOORD4", 4, DXGI_FORMAT_R32G32_FLOAT, 8, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "PRIMITIVEID", 0, DXGI_FORMAT_R32_UINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
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
	eastl::string MaterialName;

	EShadingModel ShadingModel = EShadingModel::Unknown;
	ETextureMapping TextureMapping = ETextureMapping::Unknown;
	int32 UVIndex = -1;
	float Blend = -1.0f;
	ETextureOp TextureOp = ETextureOp::Unknown;
	ETextureMapMode TextureMapMode[2]{ ETextureMapMode::Unknown, ETextureMapMode::Unknown };

	eastl::shared_ptr<FD3D12Texture2DResource> DiffuseTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> SpecularTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> AmbientTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> EmissiveTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> HeightTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> NormalsTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ShinessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> OpacityTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> DisplacementTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ReflectionTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> BaseColorTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> NormalCameraTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> EmissionColorTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> MetalnessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> DiffuseRoughnessTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> AmbientOcclusionTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> SheenTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> ClearcoatTexture;
	eastl::shared_ptr<FD3D12Texture2DResource> TransmissionTexture;

	bool bTwoSided = false;
	Vector3 ConstantDiffuse{ 1.0f, 1.0f, 1.0f };
	Vector3 ConstantEmissiveColor{ 1.0f, 1.0f, 1.0f };
	Vector3 ConstantAmbientColor{ 1.0f, 1.0f, 1.0f };
	Vector3 ConstantSpecularColor{ 1.0f, 1.0f, 1.0f };
	float ConstantSpecularFactor{ 1.0f };
	float ConstantShininess{ 1.0f };
	Vector3 ConstantTransparentColor{ 1.0f, 1.0f, 1.0f };
	float ConstantTransparencyFactor{ 1.0f };
	float ConstantOpacity{ 1.0f };
	Vector3 ConstantReflectionColor{ 1.0f };
	float ConstantReflectionFactor{ 1.0f };
	float ConstantBumpFactor{ 1.0f };
	float ConstantDisplacementFactor{ 1.0f };

	Vector3 ConstantBaseColor{ 1.0f, 1.0f, 1.0f };
	float ConstantMetalicFactor{ 0.0f };
	float ConstantRoughnessFactor{ 0.73f };
	float ConstantEmissiveIntensity{ 1.0f };

	float UseColorMap{ 0.0f };
	float UseMetalicMap{ 0.0f };
	float UseRoughnessMap{ 0.0f };
	float UseEmissiveMap{ 0.0f };
	float UseAOMap{ 0.0f };
};

struct FMeshModelCustomData
{
	FTransform Transform;
};

struct FMeshModel
{
	eastl::shared_ptr<FMesh> Mesh;
	eastl::shared_ptr<FMaterial> Material;
	eastl::vector<Matrix> InstanceLocalToWorldMatrixList;

	FMeshModelCustomData CustomData;
};