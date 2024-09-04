#include "MeshLoader.h"

#include "AssetManager.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "TextureLoader.h"
#include "Utils/ConsoleVariable.h"

#include  <filesystem>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

ETextureMapping ConvertaiTextureMappingToETextureMapping(const aiTextureMapping InaiTextureMapping)
{
	switch (InaiTextureMapping)
	{
	case aiTextureMapping::aiTextureMapping_UV:
		return ETextureMapping::UV;
	case aiTextureMapping::aiTextureMapping_SPHERE:
		return ETextureMapping::Sphere;
	case aiTextureMapping::aiTextureMapping_CYLINDER:
		return ETextureMapping::Cylinder;
	case aiTextureMapping::aiTextureMapping_BOX:
		return ETextureMapping::Box;
	case aiTextureMapping::aiTextureMapping_PLANE:
		return ETextureMapping::Plane;
	case aiTextureMapping::aiTextureMapping_OTHER:
	default:
		return ETextureMapping::Unknown;
	}
}

ETextureOp ConvertaiTextureOpToETextureOp(const aiTextureOp InaiTextureOp)
{
	switch (InaiTextureOp)
	{
	case aiTextureOp::aiTextureOp_Multiply:
		return ETextureOp::Multiply;
	case aiTextureOp::aiTextureOp_Add:
		return ETextureOp::Add;
	case aiTextureOp::aiTextureOp_Subtract:
		return ETextureOp::Subtract;
	case aiTextureOp::aiTextureOp_Divide:
		return ETextureOp::Divide;
	case aiTextureOp::aiTextureOp_SmoothAdd:
		return ETextureOp::SmoothAdd;
	case aiTextureOp::aiTextureOp_SignedAdd:
		return ETextureOp::SignedAdd;
	default:
		return ETextureOp::Unknown;
	}
}

ETextureMapMode ConvertaiTextureMapModeToETextureMapMode(const aiTextureMapMode InaiTextureMapMode)
{
	switch (InaiTextureMapMode)
	{
	case aiTextureMapMode::aiTextureMapMode_Wrap:
		return ETextureMapMode::Wrap;
	case aiTextureMapMode::aiTextureMapMode_Clamp:
		return ETextureMapMode::Clamp;
	case aiTextureMapMode::aiTextureMapMode_Decal:
		return ETextureMapMode::Decal;
	case aiTextureMapMode::aiTextureMapMode_Mirror:
		return ETextureMapMode::Mirror;
	default:
		return ETextureMapMode::Unknown;
	}
}

eastl::hash_map<const aiMesh*, eastl::shared_ptr<FMesh>> CachedMeshMap{};
eastl::hash_map<const aiMaterial*, eastl::shared_ptr<FMaterial>> CachedMaterialMap{};

eastl::shared_ptr<FMesh> CreateMesh(
	FD3D12CommandContext& InCommandContext, 
	const wchar_t* const InRelativePath, 
	eastl::shared_ptr<Assimp::Importer>& AssimpImporter, 
	const aiMesh* const AssimpMesh,
	const EMeshLoadFlags InMeshLoadFlags
)
{
	eastl::shared_ptr<FMesh> Mesh{};

	auto CachedMeshIter = CachedMeshMap.find(AssimpMesh);
	if(CachedMeshIter != CachedMeshMap.end())
	{
		Mesh = CachedMeshIter->second;
	}
	else
	{
		Mesh = eastl::make_shared<FMesh>();
		eastl::wstring MeshName = AssimpMesh->mName.C_Str() ? ANSI_TO_WCHAR(AssimpMesh->mName.C_Str()) : InRelativePath;

		// @TODO : Pool mesh vertex buffer
		Mesh->MeshName = MeshName;
		Mesh->PositionBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
			reinterpret_cast<const uint8_t*>(AssimpMesh->mVertices), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
#if D3D_NAME_OBJECT
		Mesh->PositionBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(VertexBuffer)")).c_str());
#endif

		Mesh->NormalBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
			reinterpret_cast<const uint8_t*>(AssimpMesh->mNormals), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
#if D3D_NAME_OBJECT
		Mesh->NormalBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(NormalBuffer)")).c_str());
#endif

		Mesh->TangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
			reinterpret_cast<const uint8_t*>(AssimpMesh->mTangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
#if D3D_NAME_OBJECT
		Mesh->TangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(TangentBuffer)")).c_str());
#endif

		Mesh->BiTangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
			reinterpret_cast<const uint8_t*>(AssimpMesh->mBitangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
#if D3D_NAME_OBJECT
		Mesh->BiTangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(BiTangentBuffer)")).c_str());
#endif

		static_assert(MAX_NUMBER_OF_VERTEXCOLOR == AI_MAX_NUMBER_OF_COLOR_SETS);
		static const wchar_t* const VertexColorsDebugName[MAX_NUMBER_OF_VERTEXCOLOR]{
		  EA_WCHAR("(VertexColor 0)"),
		  EA_WCHAR("(VertexColor 1)"),
		  EA_WCHAR("(VertexColor 2)"),
		  EA_WCHAR("(VertexColor 3)"),
		  EA_WCHAR("(VertexColor 4)"),
		  EA_WCHAR("(VertexColor 5)"),
		  EA_WCHAR("(VertexColor 6)"),
		  EA_WCHAR("(VertexColor 7)")
		};
		for (uint32_t VertexColorIndex = 0; VertexColorIndex < MAX_NUMBER_OF_VERTEXCOLOR; ++VertexColorIndex)
		{
			if (AssimpMesh->HasVertexColors(VertexColorIndex))
			{
				eastl::vector<uint8_t> VertexColors;  // @todo : doesn't need default initialize
				VertexColors.resize(AssimpMesh->mNumVertices * sizeof(Vector3));

				for (uint32_t VertexIndex = 0; VertexIndex < AssimpMesh->mNumVertices; ++VertexIndex)
				{
					EA::StdC::Memcpy(VertexColors.data() + VertexIndex * sizeof(Vector3), AssimpMesh->mColors[VertexColorIndex] + VertexIndex, sizeof(Vector3));
				}

				eastl::unique_ptr<FD3D12SubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(VertexColors));

				Mesh->VertexColorBuffer[VertexColorIndex] = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(Vector2));
#if D3D_NAME_OBJECT
				Mesh->VertexColorBuffer[VertexColorIndex]->SetDebugNameToResource((MeshName + VertexColorsDebugName[VertexColorIndex]).c_str());
#endif
			}
		}

		static_assert(MAX_NUMBER_OF_TEXTURECOORDS <= AI_MAX_NUMBER_OF_TEXTURECOORDS);
		static const wchar_t* const TextureCoordsDebugName[MAX_NUMBER_OF_TEXTURECOORDS]{
			EA_WCHAR("(TextureCoords 0)")
			//EA_WCHAR("(TextureCoords 1)"),
			//EA_WCHAR("(TextureCoords 2)"),
			//EA_WCHAR("(TextureCoords 3)"),
			//EA_WCHAR("(TextureCoords 4)")
		};
		for (uint32_t UVIndex = 0; UVIndex < MAX_NUMBER_OF_TEXTURECOORDS; ++UVIndex)
		{
			if (AssimpMesh->HasTextureCoords(UVIndex))
			{
				eastl::vector<uint8_t> TextureCoords;  // @todo : doesn't need default initialize
				TextureCoords.resize(AssimpMesh->mNumVertices * sizeof(Vector2));

				for (uint32_t TextureCoordIndex = 0; TextureCoordIndex < AssimpMesh->mNumVertices; ++TextureCoordIndex)
				{
					Vector2 TexCoord{ AssimpMesh->mTextureCoords[UVIndex][TextureCoordIndex].x, AssimpMesh->mTextureCoords[UVIndex][TextureCoordIndex].y };
					
					if(InMeshLoadFlags & EMeshLoadFlags::SubstractOneFromU)
					{
						TexCoord.x -= 1;
					}
					if (InMeshLoadFlags & EMeshLoadFlags::SubstractOneFromV)
					{
						TexCoord.y -= 1;
					}
					if (InMeshLoadFlags & EMeshLoadFlags::FlipU)
					{
						TexCoord.x -= 1;
						TexCoord.x *= -1;
					}
					if (InMeshLoadFlags & EMeshLoadFlags::FlipV)
					{
						TexCoord.y -= 1;
						TexCoord.y *= -1;
					}

					if (TexCoord.x < 0.0f || TexCoord.x > 1.0f || TexCoord.y < 0.0f || TexCoord.y > 1.0f)
					{
						Mesh->TexCoordOutOfRange = true;
					}

					EA::StdC::Memcpy(TextureCoords.data() + TextureCoordIndex * sizeof(Vector2), &TexCoord, sizeof(Vector2));
				}

				eastl::unique_ptr<FD3D12SubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(TextureCoords));

				Mesh->TexCoordBuffers[UVIndex] = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(Vector2));
#if D3D_NAME_OBJECT
				Mesh->TexCoordBuffers[UVIndex]->SetDebugNameToResource((MeshName + TextureCoordsDebugName[UVIndex]).c_str());
#endif
			}
		}

		if (AssimpMesh->HasFaces())
		{
			static_assert(sizeof(uint32_t) == sizeof(unsigned int));

			eastl::vector<uint8_t> IndexList{};
			IndexList.resize(AssimpMesh->mNumFaces * sizeof(uint32_t) * 3); // @todo : doesn't need default initialize
			for (uint32_t FaceIndex = 0; FaceIndex < AssimpMesh->mNumFaces; ++FaceIndex)
			{
				EA_ASSERT(AssimpMesh->mFaces[FaceIndex].mNumIndices == 3);

				EA::StdC::Memcpy(IndexList.data() + FaceIndex * sizeof(uint32_t) * 3, AssimpMesh->mFaces[FaceIndex].mIndices, sizeof(uint32_t) * 3);
			}
			eastl::unique_ptr<FD3D12SubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(IndexList));

			Mesh->IndexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticIndexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(uint32_t));
#if D3D_NAME_OBJECT
			Mesh->IndexBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(IndexBuffer)")).c_str());
#endif

			Mesh->IndexCount = AssimpMesh->mNumFaces * 3;
		}

		{
			XMFLOAT3 Min{ AssimpMesh->mAABB.mMin.x, AssimpMesh->mAABB.mMin.y, AssimpMesh->mAABB.mMin.z };
			XMFLOAT3 Max{ AssimpMesh->mAABB.mMax.x, AssimpMesh->mAABB.mMax.y, AssimpMesh->mAABB.mMax.z };
			Mesh->AABB = BoundingBox::CreateBoundingBoxFromMinMax(Min, Max);
		}

		Mesh->MaterialIndex = AssimpMesh->mMaterialIndex;

		{
			eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList{};

			VertexBufferViewList.emplace_back(Mesh->PositionBuffer->GetVertexBufferView());
			VertexBufferViewList.emplace_back(Mesh->NormalBuffer->GetVertexBufferView());
			VertexBufferViewList.emplace_back(Mesh->TangentBuffer->GetVertexBufferView());
			VertexBufferViewList.emplace_back(Mesh->BiTangentBuffer->GetVertexBufferView());

			for (uint32_t UVIndex = 0; UVIndex < MAX_NUMBER_OF_TEXTURECOORDS; ++UVIndex)
			{
				if (Mesh->TexCoordBuffers[UVIndex])
				{
					VertexBufferViewList.emplace_back(Mesh->TexCoordBuffers[UVIndex]->GetVertexBufferView());
				}
			}

			Mesh->VertexBufferViewList = VertexBufferViewList;
		}

		{
			Mesh->IndexBufferView = Mesh->IndexBuffer->GetIndexBufferView();
		}

		CachedMeshMap.emplace(AssimpMesh, Mesh);
	}
	return Mesh;
}

eastl::shared_ptr<FMaterial> CreateMaterial(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePath, const aiMaterial* const AssimpMaterial, const EMeshLoadFlags InMeshLoadFlags)
{
	eastl::shared_ptr<FMaterial> Material{};

	auto CachedMaterialIter = CachedMaterialMap.find(AssimpMaterial);
	if (CachedMaterialIter != CachedMaterialMap.end())
	{
		Material = CachedMaterialIter->second;
	}
	else
	{
		Material = eastl::make_shared<FMaterial>();
		Material->MaterialName = AssimpMaterial->GetName().C_Str();

		aiTextureType TextureTypes[] = {
			aiTextureType::aiTextureType_DIFFUSE,
			aiTextureType::aiTextureType_SPECULAR,
			aiTextureType::aiTextureType_AMBIENT,
			aiTextureType::aiTextureType_EMISSIVE,
			aiTextureType::aiTextureType_HEIGHT,
			aiTextureType::aiTextureType_NORMALS,
			aiTextureType::aiTextureType_SHININESS,
			aiTextureType::aiTextureType_OPACITY,
			aiTextureType::aiTextureType_DISPLACEMENT,
			aiTextureType::aiTextureType_REFLECTION,
			aiTextureType::aiTextureType_BASE_COLOR,
			aiTextureType::aiTextureType_NORMAL_CAMERA,
			aiTextureType::aiTextureType_EMISSION_COLOR,
			aiTextureType::aiTextureType_METALNESS,
			aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS,
			aiTextureType::aiTextureType_AMBIENT_OCCLUSION,
			aiTextureType::aiTextureType_SHEEN,
			aiTextureType::aiTextureType_CLEARCOAT,
			aiTextureType::aiTextureType_TRANSMISSION
		};

		for (uint32_t TextureTypeIndex = 0; TextureTypeIndex < ARRAY_LENGTH(TextureTypes); ++TextureTypeIndex)
		{
			const aiTextureType TextureType = TextureTypes[TextureTypeIndex];
			const uint32_t TextureCount = AssimpMaterial->GetTextureCount(TextureType);

			for (uint32_t TextureIndex = 0; TextureIndex < TextureCount; ++TextureIndex)
			{
				struct FAssimpMaterialProperty
				{
					aiString Path;
					aiTextureMapping Mapping;
					unsigned int UVindex;
					ai_real Blend;
					aiTextureOp TextureOP;
					aiTextureMapMode MapMode[2];
				} MaterialProperty;

				EA::StdC::Memset8(&MaterialProperty, 0xFF, sizeof(MaterialProperty));

				if (aiReturn::aiReturn_SUCCESS == AssimpMaterial->GetTexture(TextureType, TextureIndex,
					&MaterialProperty.Path,
					&MaterialProperty.Mapping,
					&MaterialProperty.UVindex,
					&MaterialProperty.Blend,
					&MaterialProperty.TextureOP,
					MaterialProperty.MapMode)
					)
				{

					Material->TextureMapping = ConvertaiTextureMappingToETextureMapping(MaterialProperty.Mapping);
					Material->UVIndex = MaterialProperty.UVindex;
					if (MaterialProperty.Blend >= 0.0f && MaterialProperty.Blend <= 1.0f)
					{
						Material->Blend = MaterialProperty.Blend;
					}
					else
					{
						Material->Blend = -1.0f;
					}
					Material->TextureOp = ConvertaiTextureOpToETextureOp(MaterialProperty.TextureOP);
					Material->TextureMapMode[0] = ConvertaiTextureMapModeToETextureMapMode(MaterialProperty.MapMode[0]);
					Material->TextureMapMode[1] = ConvertaiTextureMapModeToETextureMapMode(MaterialProperty.MapMode[1]);

					eastl::wstring TextureFileRelativePath{ std::filesystem::path{ InRelativePath }.parent_path().c_str() };
					TextureFileRelativePath += EA_WCHAR("/");
					TextureFileRelativePath += ANSI_TO_WCHAR(MaterialProperty.Path.C_Str());

					eastl::shared_ptr<FD3D12Texture2DResource> TextureResource = FTextureLoader::LoadTexture2DFromFile(InCommandContext,
						TextureFileRelativePath.c_str(),
						D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
						DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
						D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					);

					switch (TextureType)
					{
					case aiTextureType::aiTextureType_DIFFUSE:
						Material->DiffuseTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_SPECULAR:
						Material->SpecularTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_AMBIENT:
						Material->AmbientTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_EMISSIVE:
						Material->EmissiveTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_HEIGHT:
						Material->HeightTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_NORMALS:
						Material->NormalsTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_SHININESS:
						Material->ShinessTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_OPACITY:
						Material->OpacityTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_DISPLACEMENT:
						Material->DisplacementTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_REFLECTION:
						Material->ReflectionTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_BASE_COLOR:
						Material->BaseColorTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_NORMAL_CAMERA:
						Material->NormalCameraTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_EMISSION_COLOR:
						Material->EmissionColorTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_METALNESS:
						Material->MetalnessTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
						Material->DiffuseRoughnessTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
						Material->AmbientOcclusionTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_SHEEN:
						Material->SheenTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_CLEARCOAT:
						Material->ClearcoatTexture = TextureResource;
						break;
					case aiTextureType::aiTextureType_TRANSMISSION:
						Material->TransmissionTexture = TextureResource;
						break;
					default:
						EA_ASSUME(false);
						break;
					}
				}
			}
		}

		AssimpMaterial->Get(AI_MATKEY_TWOSIDED, Material->bTwoSided);

		aiColor3D DiffuseColor;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == aiReturn_SUCCESS)
		{
			Material->ConstantDiffuse = Vector3(DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		}

		aiColor3D SpecularColor;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == aiReturn_SUCCESS)
		{
			Material->ConstantSpecularColor = Vector3(SpecularColor.r, SpecularColor.g, SpecularColor.b);
		}

		float SpecularFactor;
		if (AssimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, SpecularFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantSpecularFactor = SpecularFactor;
		}

		float Shiness;
		if (AssimpMaterial->Get(AI_MATKEY_SHININESS, Shiness) == aiReturn_SUCCESS)
		{
			Material->ConstantShininess = Shiness;
		}

		aiColor3D Transparent;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, Transparent) == aiReturn_SUCCESS)
		{
			Material->ConstantTransparentColor = Vector3(Transparent.r, Transparent.g, Transparent.b);
		}

		float TransparentFactor;
		if (AssimpMaterial->Get(AI_MATKEY_TRANSPARENCYFACTOR, TransparentFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantTransparencyFactor = TransparentFactor;
		}

		float Opacity;
		if (AssimpMaterial->Get(AI_MATKEY_OPACITY, Opacity) == aiReturn_SUCCESS)
		{
			Material->ConstantOpacity = Opacity;
		}

		aiColor3D ReflectionColor;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, ReflectionColor) == aiReturn_SUCCESS)
		{
			Material->ConstantReflectionColor = Vector3(ReflectionColor.r, ReflectionColor.g, ReflectionColor.b);
		}

		float ReflectionFactor;
		if (AssimpMaterial->Get(AI_MATKEY_REFLECTIVITY, ReflectionFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantReflectionFactor = ReflectionFactor;
		}

		float BumpFactor;
		if (AssimpMaterial->Get(AI_MATKEY_BUMPSCALING, BumpFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantBumpFactor = BumpFactor;
		}

		aiColor3D BaseColor;
		if (AssimpMaterial->Get(AI_MATKEY_BASE_COLOR, BaseColor) == aiReturn_SUCCESS)
		{
			Material->ConstantBaseColor = Vector3(BaseColor.r, BaseColor.g, BaseColor.b);
		}

		float UseColorMap;
		if (AssimpMaterial->Get(AI_MATKEY_USE_COLOR_MAP, UseColorMap) == aiReturn_SUCCESS)
		{
			Material->UseColorMap = UseColorMap;
		}

		float MetalicFactor;
		if (AssimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, MetalicFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantMetalicFactor = MetalicFactor;
		}

		float UseMetalicMap;
		if (AssimpMaterial->Get(AI_MATKEY_USE_METALLIC_MAP, UseMetalicMap) == aiReturn_SUCCESS)
		{
			Material->UseMetalicMap = UseMetalicMap;
		}

		float RoughnessFactor;
		if (AssimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, RoughnessFactor) == aiReturn_SUCCESS)
		{
			Material->ConstantRoughnessFactor = RoughnessFactor;
		}

		float UseRoughnessMap;
		if (AssimpMaterial->Get(AI_MATKEY_USE_ROUGHNESS_MAP, UseRoughnessMap) == aiReturn_SUCCESS)
		{
			Material->UseRoughnessMap = UseRoughnessMap;
		}

		float EmissiveIntensity;
		if (AssimpMaterial->Get(AI_MATKEY_EMISSIVE_INTENSITY, EmissiveIntensity) == aiReturn_SUCCESS)
		{
			Material->ConstantEmissiveIntensity = EmissiveIntensity;
		}

		aiColor3D EmissiveColor;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, EmissiveColor) == aiReturn_SUCCESS)
		{
			Material->ConstantEmissiveColor = Vector3(EmissiveColor.r, EmissiveColor.g, EmissiveColor.b);
		}

		float UseEmissiveMap;
		if (AssimpMaterial->Get(AI_MATKEY_USE_EMISSIVE_MAP, UseEmissiveMap) == aiReturn_SUCCESS)
		{
			Material->UseEmissiveMap = UseEmissiveMap;
		}

		aiColor3D AmbientColor;
		if (AssimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == aiReturn_SUCCESS)
		{
			Material->ConstantAmbientColor = Vector3(AmbientColor.r, AmbientColor.g, AmbientColor.b);
		}

		float UseAOMap;
		if (AssimpMaterial->Get(AI_MATKEY_USE_AO_MAP, UseAOMap) == aiReturn_SUCCESS)
		{
			Material->UseAOMap = UseAOMap;
		}

		CachedMaterialMap.emplace(AssimpMaterial, Material);
	}

	return Material;
}

void PostProcessMeshModel(FMeshModel& NewMeshModel, const EMeshLoadFlags InMeshLoadFlags)
{
	if (InMeshLoadFlags & EMeshLoadFlags::MirrorAddressModeIfTextureCoordinatesOutOfRange)
	{
		if (NewMeshModel.Mesh->TexCoordOutOfRange)
		{
			NewMeshModel.Material->TextureMapMode[0] = ETextureMapMode::Mirror;
			NewMeshModel.Material->TextureMapMode[1] = ETextureMapMode::Mirror;
		}
	}
}
void TraverseAssimpScene(
	eastl::hash_map<const aiMesh*, int32> CachedMeshModelMap,
	FD3D12CommandContext& InCommandContext,
	const wchar_t* const InRelativePath,
	const aiScene* const InAssimpScene, 
	eastl::shared_ptr<Assimp::Importer>& AssimpImporter,
	const aiNode* const InAiNode, 
	const EMeshLoadFlags InMeshLoadFlags,
	const FMeshModelCustomData& InCustomData,
	eastl::vector<FMeshModel>& OutMeshModelList
)
{
	for (int32 MeshIndex = 0; MeshIndex < InAiNode->mNumMeshes; ++MeshIndex)
	{
		const aiMesh* const AssimpMesh = InAssimpScene->mMeshes[InAiNode->mMeshes[MeshIndex]];
		FMeshModel* NewMeshModel = nullptr;

		auto CacehdMeshModelIter = CachedMeshModelMap.find(AssimpMesh);
		if (CacehdMeshModelIter != CachedMeshModelMap.end())
		{
			NewMeshModel = &OutMeshModelList[CacehdMeshModelIter->second];
		}
		else
		{
			const aiMaterial* const AssimpMaterial = InAssimpScene->mMaterials[AssimpMesh->mMaterialIndex];

			NewMeshModel = &OutMeshModelList.emplace_back();
			NewMeshModel->Mesh = CreateMesh(InCommandContext, InRelativePath, AssimpImporter, AssimpMesh, InMeshLoadFlags);
			NewMeshModel->Material = CreateMaterial(InCommandContext, InRelativePath, AssimpMaterial, InMeshLoadFlags);
			NewMeshModel->CustomData = InCustomData;

			PostProcessMeshModel(*NewMeshModel, InMeshLoadFlags);

			CachedMeshModelMap.emplace(AssimpMesh, OutMeshModelList.size() - 1);
		}


		Matrix LocalToWorldMatrix{
			InAiNode->mTransformation.a1, InAiNode->mTransformation.a2, InAiNode->mTransformation.a3, InAiNode->mTransformation.a4,
			InAiNode->mTransformation.b1, InAiNode->mTransformation.b2, InAiNode->mTransformation.b3, InAiNode->mTransformation.b4,
			InAiNode->mTransformation.c1, InAiNode->mTransformation.c2, InAiNode->mTransformation.c3, InAiNode->mTransformation.c4,
			InAiNode->mTransformation.d1, InAiNode->mTransformation.d2, InAiNode->mTransformation.d3, InAiNode->mTransformation.d4
		};
		NewMeshModel->InstanceLocalToWorldMatrixList.emplace_back(LocalToWorldMatrix);
	}

	for(int32 ChildIndex = 0 ; ChildIndex < InAiNode->mNumChildren ; ++ChildIndex)
	{
		TraverseAssimpScene(
			CachedMeshModelMap,
			InCommandContext, 
			InRelativePath, 
			InAssimpScene, 
			AssimpImporter, 
			InAiNode->mChildren[ChildIndex],
			InMeshLoadFlags, 
			InCustomData,
			OutMeshModelList
		);
	}
}

eastl::vector<FMeshModel> FMeshLoader::LoadFromMeshFile(
	FD3D12CommandContext& InCommandContext,
	const wchar_t* const InRelativePath,
	const FMeshModelCustomData& InCustomData,
	const EMeshLoadFlags InMeshLoadFlags
)
{
	RD_LOG(ELogVerbosity::Log, EA_WCHAR("Loading Mesh(%s)"), InRelativePath);

    eastl::vector<FMeshModel> MeshModelList{};

    // Create an instance of the Importer class
    eastl::shared_ptr<Assimp::Importer> AssimpImporter = eastl::make_shared<Assimp::Importer>();

	unsigned int PostProcessSteps = aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs |
		aiProcess_GenBoundingBoxes;

	if (InMeshLoadFlags & EMeshLoadFlags::DontConvertToLeftHand)
	{
		PostProcessSteps &= ~aiProcess_ConvertToLeftHanded;
	}

	if (InMeshLoadFlags & EMeshLoadFlags::DontFlipUVs)
	{
		PostProcessSteps &= ~aiProcess_FlipUVs;
	}

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
	const aiScene* const AssimpScene = AssimpImporter->ReadFile(WCHAR_TO_UTF8(FAssetManager::MakeAbsolutePathFromAssetFolder(InRelativePath)), PostProcessSteps);

    // If the import failed, report it
    if (AssimpScene) 
	{
		eastl::hash_map<const aiMesh*, int32> CachedMeshModelMap{};

		TraverseAssimpScene(
			CachedMeshModelMap, 
			InCommandContext, 
			InRelativePath, 
			AssimpScene, 
			AssimpImporter, 
			AssimpScene->mRootNode,
			InMeshLoadFlags,
			InCustomData,
			MeshModelList
		);

		RD_LOG(ELogVerbosity::Log, EA_WCHAR("Success to load Mesh(%s)"), InRelativePath);
    }
    else
	{
		RD_LOG(ELogVerbosity::Log, EA_WCHAR("Fail to load Mesh(%s)"), InRelativePath);
        EA_ASSERT_FORMATTED(false, ("Assimp ReadfFile Fail : %s", AssimpImporter->GetErrorString()));
    }

    return MeshModelList;
}
