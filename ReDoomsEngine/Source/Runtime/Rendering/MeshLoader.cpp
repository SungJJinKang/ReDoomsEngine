#include "MeshLoader.h"

#include "AssetManager.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "TextureLoader.h"

#include  <filesystem>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

eastl::shared_ptr<F3DModel> FMeshLoader::LoadFromMeshFile(FD3D12CommandContext& InCommandContext, const wchar_t* const InRelativePathToAssetFolder)
{
    eastl::shared_ptr<F3DModel> Result{};

    // Create an instance of the Importer class
    eastl::shared_ptr<Assimp::Importer> AssimpImporter = eastl::make_shared<Assimp::Importer>();

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* const AssimpScene = AssimpImporter->ReadFile(WCHAR_TO_UTF8(FAssetManager::MakeAbsolutePathFromAssetFolder(InRelativePathToAssetFolder)),
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate
        );

    // If the import failed, report it
    if (AssimpScene) 
    {
        Result = eastl::make_shared<F3DModel>();

        for (uint32_t MeshIndex = 0; MeshIndex < AssimpScene->mNumMeshes; ++MeshIndex)
        {
            FMesh& Mesh = Result->MeshList.emplace_back();

            const aiMesh* const AssimpMesh = AssimpScene->mMeshes[MeshIndex];

            eastl::wstring MeshName = AssimpMesh->mName.C_Str() ? ANSI_TO_WCHAR(AssimpMesh->mName.C_Str()) : InRelativePathToAssetFolder;
            
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
                EA_WCHAR("(TextureCoords 0)"),
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

            Mesh.MaterialIndex = AssimpMesh->mMaterialIndex;
        }

        Result->Material.resize(AssimpScene->mNumMaterials);
        for (uint32_t MaterialIndex = 0; MaterialIndex < AssimpScene->mNumMaterials; ++MaterialIndex)
        {
            FMeshMaterial& MeshMaterial = Result->Material[MaterialIndex];

            const aiMaterial* const AssimpMaterial = AssimpScene->mMaterials[MaterialIndex];

            aiTextureType TextureTypes[] = {
                aiTextureType::aiTextureType_DIFFUSE,
                aiTextureType::aiTextureType_EMISSIVE,
                aiTextureType::aiTextureType_SHININESS,
                aiTextureType::aiTextureType_METALNESS,
                aiTextureType::aiTextureType_BASE_COLOR
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

                    AssimpMaterial->GetTexture(TextureType, TextureIndex,
                        &MaterialProperty.Path,
                        &MaterialProperty.Mapping,
                        &MaterialProperty.UVindex,
                        &MaterialProperty.Blend,
                        &MaterialProperty.TextureOP,
                        MaterialProperty.MapMode
                    );


					std::filesystem::path ShaderTextFilePath{ InRelativePathToAssetFolder };
                  
                    eastl::wstring TextureRelativePath = ShaderTextFilePath.parent_path().c_str();
                    TextureRelativePath += EA_WCHAR("/");
                    TextureRelativePath += ANSI_TO_WCHAR(MaterialProperty.Path.C_Str());

                    eastl::shared_ptr<FD3D12Texture2DResource> TextureResource = FTextureLoader::LoadFromFile(InCommandContext,
                        TextureRelativePath.c_str(),
                        D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
                        DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
                        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
                    );

                    switch (TextureType)
                    {
                    case aiTextureType_DIFFUSE:
                        MeshMaterial.DiffuseTexture = TextureResource;
                        break;
					case aiTextureType_EMISSIVE:
						MeshMaterial.EmissiveTexture = TextureResource;
                        break;
					case aiTextureType_SHININESS:
						MeshMaterial.ShinessTexture = TextureResource;
                        break;
					case aiTextureType_METALNESS:
						MeshMaterial.MetalnessTexture = TextureResource;
                        break;
                    default:
                        EA_ASSUME(false);
                        break;
                    }
                }
            }

        }

    }
    else
    {
        EA_ASSERT_FORMATTED(false, ("Assimp ReadfFile Fail : %s", AssimpImporter->GetErrorString()));
    }

    return Result;
}
