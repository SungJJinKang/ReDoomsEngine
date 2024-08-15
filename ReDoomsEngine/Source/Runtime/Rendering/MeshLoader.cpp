#include "MeshLoader.h"

#include "AssetManager.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "TextureLoader.h"

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
		return ETextureMapping::Other;
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

eastl::vector<eastl::shared_ptr<FMeshModel>> FMeshLoader::LoadFromMeshFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePath)
{
	RD_LOG(ELogVerbosity::Log, EA_WCHAR("Loading Mesh(%s)"), InRelativePath);

    eastl::vector<eastl::shared_ptr<FMeshModel>> MeshModelList{};

    // Create an instance of the Importer class
    eastl::shared_ptr<Assimp::Importer> AssimpImporter = eastl::make_shared<Assimp::Importer>();

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* const AssimpScene = AssimpImporter->ReadFile(WCHAR_TO_UTF8(FAssetManager::MakeAbsolutePathFromAssetFolder(InRelativePath)),
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_ConvertToLeftHanded
        );

    // If the import failed, report it
    if (AssimpScene) 
    {
        for (uint32_t MeshIndex = 0; MeshIndex < AssimpScene->mNumMeshes; ++MeshIndex)
        {
			eastl::shared_ptr<FMeshModel>& MeshModel = MeshModelList.emplace_back();
			MeshModel = eastl::make_shared<FMeshModel>();

			FMesh& Mesh = MeshModel->Mesh;
			FMaterial& Material = MeshModel->Material;

            const aiMesh* const AssimpMesh = AssimpScene->mMeshes[MeshIndex];

            eastl::wstring MeshName = AssimpMesh->mName.C_Str() ? ANSI_TO_WCHAR(AssimpMesh->mName.C_Str()) : InRelativePath;
            
            Mesh.MeshName = MeshName;
            Mesh.PositionBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mVertices), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            #if D3D_NAME_OBJECT
            Mesh.PositionBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(VertexBuffer)")).c_str());
            #endif

            Mesh.NormalBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mNormals), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            #if D3D_NAME_OBJECT
            Mesh.NormalBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(NormalBuffer)")).c_str());
            #endif

            Mesh.TangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mTangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            #if D3D_NAME_OBJECT
            Mesh.TangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(TangentBuffer)")).c_str());
            #endif

            Mesh.BiTangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mBitangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            #if D3D_NAME_OBJECT
            Mesh.BiTangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(BiTangentBuffer)")).c_str());
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
                    EA_ASSERT(AssimpMesh->GetNumColorChannels() == 3);

					eastl::vector<uint8_t> VertexColors;  // @todo : doesn't need default initialize
					VertexColors.resize(AssimpMesh->mNumVertices * sizeof(Vector3));

					for (uint32_t VertexIndex = 0; VertexIndex < AssimpMesh->mNumVertices; ++VertexIndex)
					{
						EA::StdC::Memcpy(VertexColors.data() + VertexIndex * sizeof(Vector3), AssimpMesh->mColors[VertexColorIndex] + VertexIndex, sizeof(Vector3));
					}

					eastl::unique_ptr<FD3D12SubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(VertexColors));

					Mesh.VertexColorBuffer[VertexColorIndex] = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(Vector2));
                    #if D3D_NAME_OBJECT
					Mesh.VertexColorBuffer[VertexColorIndex]->SetDebugNameToResource((MeshName + VertexColorsDebugName[VertexColorIndex]).c_str());
                    #endif
                }
            }
           
            static_assert(MAX_NUMBER_OF_TEXTURECOORDS <= AI_MAX_NUMBER_OF_TEXTURECOORDS);
            static const wchar_t* const TextureCoordsDebugName[MAX_NUMBER_OF_TEXTURECOORDS]{
                EA_WCHAR("(TextureCoords 0)")
				EA_WCHAR("(TextureCoords 1)"),
				EA_WCHAR("(TextureCoords 2)"),
				EA_WCHAR("(TextureCoords 3)"),
				EA_WCHAR("(TextureCoords 4)")
            };
            for (uint32_t UVIndex = 0; UVIndex < MAX_NUMBER_OF_TEXTURECOORDS; ++UVIndex)
            {
                if (AssimpMesh->HasTextureCoords(UVIndex))
                {
                    eastl::vector<uint8_t> TextureCoords;  // @todo : doesn't need default initialize
                    TextureCoords.resize(AssimpMesh->mNumVertices * sizeof(Vector2));

                    for (uint32_t TextureCoordIndex = 0; TextureCoordIndex < AssimpMesh->mNumVertices; ++TextureCoordIndex)
                    {
                        EA::StdC::Memcpy(TextureCoords.data() + TextureCoordIndex * sizeof(Vector2), AssimpMesh->mTextureCoords[UVIndex] + TextureCoordIndex, sizeof(Vector2));
                    }

                    eastl::unique_ptr<FD3D12SubresourceContainer> SubresourceContainer = eastl::make_unique<FD3D12VertexIndexBufferSubresourceContainer>(eastl::move(TextureCoords));

                    Mesh.TexCoordBuffers[UVIndex] = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(Vector2));
                    #if D3D_NAME_OBJECT
                    Mesh.TexCoordBuffers[UVIndex]->SetDebugNameToResource((MeshName + TextureCoordsDebugName[UVIndex]).c_str());
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

                Mesh.IndexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticIndexBuffer(InCommandContext, eastl::move(SubresourceContainer), sizeof(uint32_t));
                #if D3D_NAME_OBJECT
                Mesh.IndexBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(IndexBuffer)")).c_str());
                #endif

                Mesh.IndexCount = AssimpMesh->mNumFaces * 3;
            }

            {
                XMFLOAT3 Min{ AssimpMesh->mAABB.mMin.x, AssimpMesh->mAABB.mMin.y, AssimpMesh->mAABB.mMin.z };
                XMFLOAT3 Max{ AssimpMesh->mAABB.mMax.x, AssimpMesh->mAABB.mMax.y, AssimpMesh->mAABB.mMax.z };
                Mesh.AABB = BoundingBox::CreateBoundingBoxFromMinMax(Min, Max);
            }

            Mesh.MaterialIndex = AssimpMesh->mMaterialIndex;

            {
                eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> VertexBufferViewList{};

                VertexBufferViewList.emplace_back(Mesh.PositionBuffer->GetVertexBufferView());
                VertexBufferViewList.emplace_back(Mesh.NormalBuffer->GetVertexBufferView());
                VertexBufferViewList.emplace_back(Mesh.TangentBuffer->GetVertexBufferView());
                VertexBufferViewList.emplace_back(Mesh.BiTangentBuffer->GetVertexBufferView());

                for (uint32_t UVIndex = 0; UVIndex < MAX_NUMBER_OF_TEXTURECOORDS; ++UVIndex)
                {
                    if (Mesh.TexCoordBuffers[UVIndex])
                    {
                        VertexBufferViewList.emplace_back(Mesh.TexCoordBuffers[UVIndex]->GetVertexBufferView());
                    }
                }

                Mesh.VertexBufferViewList = VertexBufferViewList;
            }

            {
                Mesh.IndexBufferView = Mesh.IndexBuffer->GetIndexBufferView();
            }

			
			const aiMaterial* const AssimpMaterial = AssimpScene->mMaterials[AssimpMesh->mMaterialIndex];

            aiTextureType TextureTypes[] = {
                aiTextureType::aiTextureType_BASE_COLOR,
				aiTextureType::aiTextureType_NORMAL_CAMERA,
				aiTextureType::aiTextureType_EMISSION_COLOR,
				aiTextureType::aiTextureType_METALNESS,
				aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType::aiTextureType_AMBIENT_OCCLUSION,
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

						Material.TextureMapping = ConvertaiTextureMappingToETextureMapping(MaterialProperty.Mapping);
						Material.UVIndex = MaterialProperty.UVindex;
						if (MaterialProperty.Blend >= 0.0f && MaterialProperty.Blend <= 1.0f)
						{
							Material.Blend = MaterialProperty.Blend;
						}
						else
						{
							Material.Blend = -1.0f;
						}
						Material.TextureOp = ConvertaiTextureOpToETextureOp(MaterialProperty.TextureOP);
						Material.TextureMapMode[0] = ConvertaiTextureMapModeToETextureMapMode(MaterialProperty.MapMode[0]);
						Material.TextureMapMode[1] = ConvertaiTextureMapModeToETextureMapMode(MaterialProperty.MapMode[1]);

						eastl::wstring TextureFileRelativePath{ std::filesystem::path{ InRelativePath }.parent_path().c_str() };
						TextureFileRelativePath += EA_WCHAR("/");
						TextureFileRelativePath += ANSI_TO_WCHAR(MaterialProperty.Path.C_Str());

						eastl::shared_ptr<FD3D12Texture2DResource> TextureResource = FTextureLoader::LoadFromFile(InCommandContext,
							TextureFileRelativePath.c_str(),
							D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
							DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
							D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
						);

						switch (TextureType)
						{
							case aiTextureType::aiTextureType_BASE_COLOR:
								Material.BaseColor = TextureResource;
								break;
							case aiTextureType::aiTextureType_EMISSION_COLOR:
								Material.Emissive = TextureResource;
								break;
							case aiTextureType::aiTextureType_METALNESS:
								Material.Metalic = TextureResource;
								break;
							case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
								Material.Roughness = TextureResource;
								break;
							case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
								Material.AmbientOcclusion = TextureResource;
								break;
							default:
								EA_ASSUME(false);
								break;
						}
					}
                }
            }

        }

		RD_LOG(ELogVerbosity::Log, EA_WCHAR("Success to load Mesh(%s)"), InRelativePath);
    }
    else
	{
		RD_LOG(ELogVerbosity::Log, EA_WCHAR("Fail to load Mesh(%s)"), InRelativePath);
        EA_ASSERT_FORMATTED(false, ("Assimp ReadfFile Fail : %s", AssimpImporter->GetErrorString()));
    }

    return MeshModelList;
}
