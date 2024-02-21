#include "MeshLoader.h"

#include "AssetManager.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"

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
        aiProcess_MakeLeftHanded | // directx is left-handed
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (AssimpScene) 
    {
        Result = eastl::make_shared<F3DModel>();

        for (uint32_t MeshIndex = 0; MeshIndex < AssimpScene->mNumMeshes; ++MeshIndex)
        {
            FMesh& Mesh = Result->MeshList.emplace_back();

            const aiMesh* const AssimpMesh = AssimpScene->mMeshes[MeshIndex];

            eastl::wstring MeshName = AssimpMesh->mName.C_Str() ? ANSI_TO_WCHAR(AssimpMesh->mName.C_Str()) : InRelativePathToAssetFolder;
            
            Mesh.PositionBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mVertices), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            Mesh.PositionBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(VertexBuffer)")).c_str());

            Mesh.NormalBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mNormals), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            Mesh.NormalBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(NormalBuffer)")).c_str());

            Mesh.TangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mTangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            Mesh.TangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(TangentBuffer)")).c_str());

            Mesh.BiTangentBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                reinterpret_cast<const uint8_t*>(AssimpMesh->mBitangents), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
            Mesh.BiTangentBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(BiTangentBuffer)")).c_str());

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
                    Mesh.UVBuffers[UVIndex] = FD3D12ResourceAllocator::GetInstance()->AllocateStaticVertexBuffer(InCommandContext,
                        reinterpret_cast<const uint8_t*>(AssimpMesh->mTextureCoords), sizeof(aiVector3D) * AssimpMesh->mNumVertices, sizeof(aiVector3D), AssimpImporter);
                    Mesh.UVBuffers[UVIndex]->SetDebugNameToResource((MeshName + TextureCoordsDebugName[UVIndex]).c_str());
                }
            }

            if (AssimpMesh->HasFaces())
            {
                static_assert(sizeof(uint32_t) == sizeof(unsigned int));

                eastl::vector<uint32_t> IndexList{};
                IndexList.resize(AssimpMesh->mNumFaces * 3);
                for (uint32_t FaceIndex = 0; FaceIndex < AssimpMesh->mNumFaces; ++FaceIndex)
                {
                    EA_ASSERT(AssimpMesh->mFaces[FaceIndex].mNumIndices == 3);

                    EA::StdC::Memcpy(IndexList.data() + FaceIndex * 3, AssimpMesh->mFaces[FaceIndex].mIndices, sizeof(uint32_t) * 3);
                }
                Mesh.IndexBuffer = FD3D12ResourceAllocator::GetInstance()->AllocateStaticIndexBuffer(InCommandContext,
                    reinterpret_cast<const uint8_t*>(IndexList.data()), IndexList.size() * sizeof(uint32_t), sizeof(uint32_t), AssimpImporter);
                Mesh.IndexBuffer->SetDebugNameToResource((MeshName + EA_WCHAR("(IndexBuffer)")).c_str());
            }
        }
    }
    else
    {
        EA_ASSERT_FORMATTED(false, ("Assimp ReadfFile Fail : %s", AssimpImporter->GetErrorString()));
    }

    return Result;
}
