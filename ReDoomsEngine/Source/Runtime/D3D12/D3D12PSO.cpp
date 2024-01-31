#include "D3D12PSO.h"
#include "D3D12Device.h"
#include "D3D12RootSignature.h"
#include "ShaderCompilers/DirectXShaderCompiler/inc/dxcapi.h"

static_assert(std::is_pod<FD3D12PSOInitializer::FDesc>::value);

void FD3D12PSOInitializer::FinishCreating()
{
    EA_ASSERT(CachedHash == 0);

    uint128 BoundShaderSetHash;
    BoundShaderSetHash.first = BoundShaderSet.CachedHash.Value[0];
    BoundShaderSetHash.second = BoundShaderSet.CachedHash.Value[1];
    const uint64 BoundShaderSetHash64 = Hash128to64(BoundShaderSetHash);
    const uint64 DescHash = CityHash64(reinterpret_cast<const char*>(&Desc), sizeof(BoundShaderSetHash));

    CachedHash = BoundShaderSetHash64 ^ DescHash;
}

FD3D12PSO::FD3D12PSO(const FD3D12PSOInitializer& InPSOInitializer)
    : PSOInitializer(InPSOInitializer)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc{};
  
    Desc.pRootSignature = FD3D12RootSignatureManager::GetInstance()->GetOrCreateRootSignature(PSOInitializer.BoundShaderSet)->RootSignature.Get();
    Desc.VS.pShaderBytecode = PSOInitializer.BoundShaderSet.ShaderList[EShaderFrequency::Vertex]->GetShaderBlob()->GetBufferPointer();
    Desc.VS.BytecodeLength = PSOInitializer.BoundShaderSet.ShaderList[EShaderFrequency::Vertex]->GetShaderBlob()->GetBufferSize();
    Desc.PS.pShaderBytecode = PSOInitializer.BoundShaderSet.ShaderList[EShaderFrequency::Pixel]->GetShaderBlob()->GetBufferPointer();
    Desc.PS.BytecodeLength = PSOInitializer.BoundShaderSet.ShaderList[EShaderFrequency::Pixel]->GetShaderBlob()->GetBufferSize();

    #define COPY_DESC_MEMBER(MemberName) Desc.MemberName = PSOInitializer.Desc.MemberName;
    COPY_DESC_MEMBER(StreamOutput)
    COPY_DESC_MEMBER(BlendState)
    COPY_DESC_MEMBER(SampleMask)
    COPY_DESC_MEMBER(RasterizerState)
    COPY_DESC_MEMBER(DepthStencilState)
    COPY_DESC_MEMBER(InputLayout)
    COPY_DESC_MEMBER(IBStripCutValue)
    COPY_DESC_MEMBER(PrimitiveTopologyType)
    COPY_DESC_MEMBER(NumRenderTargets)
    EA::StdC::Memcpy(&(Desc.RTVFormats), &(PSOInitializer.Desc.RTVFormats), sizeof(DXGI_FORMAT) * ARRAY_LENGTH(PSOInitializer.Desc.RTVFormats));
    COPY_DESC_MEMBER(DSVFormat)
    COPY_DESC_MEMBER(SampleDesc)
    COPY_DESC_MEMBER(NodeMask)
    COPY_DESC_MEMBER(CachedPSO)
    COPY_DESC_MEMBER(Flags)

    VERIFYD3D12RESULT(GetD3D12Device()->CreateGraphicsPipelineState(&Desc, IID_PPV_ARGS(&PSOObject)));
}

FD3D12PSO* FD3D12PSOManager::GetOrCreatePSO(const FD3D12PSOInitializer& InD3D12PSOInitializer)
{
    FD3D12PSO* NewPSO = nullptr;

    auto Iter = PSOHashMap.find(InD3D12PSOInitializer);
    if (Iter == PSOHashMap.end())
    {
        eastl::unique_ptr<FD3D12PSO> NewPSOUniquePtr = eastl::make_unique<FD3D12PSO>(InD3D12PSOInitializer);
        NewPSO = NewPSOUniquePtr.get();
        PSOHashMap.emplace(InD3D12PSOInitializer, eastl::move(NewPSOUniquePtr));
    }
    else
    {
        NewPSO = Iter->second.get();
    }

    EA_ASSERT(NewPSO);
    return NewPSO;
}
