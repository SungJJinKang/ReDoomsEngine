#include "D3D12PSO.h"
#include "D3D12Device.h"
#include "D3D12RootSignature.h"
#include "ShaderCompilers/DirectXShaderCompiler/include/dxc/dxcapi.h"

bool FD3D12PSOInitializer::IsValid() const
{
    return (CachedHash != 0) &&
        (DrawDesc.BoundShaderSet.GetCachedHash().Value[0] != 0) && (DrawDesc.BoundShaderSet.GetCachedHash().Value[1] != 0);
}

// void FD3D12PSOInitializer::Reset()
// {
//     MEM_ZERO(BoundShaderSet);
//     MEM_ZERO(Desc);
//     MEM_ZERO(CachedHash);
// }

void FD3D12PSOInitializer::FinishCreating()
{
    uint128 BoundShaderSetHash;
    BoundShaderSetHash.first = DrawDesc.BoundShaderSet.GetCachedHash().Value[0];
    BoundShaderSetHash.second = DrawDesc.BoundShaderSet.GetCachedHash().Value[1];
    const uint64 BoundShaderSetHash64 = Hash128to64(BoundShaderSetHash);
    const uint64 PassDescHash = CityHash64(reinterpret_cast<const char*>(&PassDesc), sizeof(PassDesc));
    const uint64 DrawDescHash = CityHash64(reinterpret_cast<const char*>(&DrawDesc.PSODesc), sizeof(DrawDesc.PSODesc));

    CachedHash = BoundShaderSetHash64 ^ PassDescHash ^ DrawDescHash;
}

FD3D12PSO::FD3D12PSO(const FD3D12PSOInitializer& InPSOInitializer)
    : PSOInitializer(InPSOInitializer)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc{};
  
    Desc.pRootSignature = PSOInitializer.DrawDesc.BoundShaderSet.GetRootSignature()->RootSignature.Get();
    Desc.VS.pShaderBytecode = PSOInitializer.DrawDesc.BoundShaderSet.GetShaderInstanceList()[EShaderFrequency::Vertex]->GetShaderTemplate()->GetShaderBlob()->GetBufferPointer();
    Desc.VS.BytecodeLength = PSOInitializer.DrawDesc.BoundShaderSet.GetShaderInstanceList()[EShaderFrequency::Vertex]->GetShaderTemplate()->GetShaderBlob()->GetBufferSize();
    Desc.PS.pShaderBytecode = PSOInitializer.DrawDesc.BoundShaderSet.GetShaderInstanceList()[EShaderFrequency::Pixel]->GetShaderTemplate()->GetShaderBlob()->GetBufferPointer();
    Desc.PS.BytecodeLength = PSOInitializer.DrawDesc.BoundShaderSet.GetShaderInstanceList()[EShaderFrequency::Pixel]->GetShaderTemplate()->GetShaderBlob()->GetBufferSize();

    #define COPY_DRAW_DESC_MEMBER(MemberName) Desc.MemberName = PSOInitializer.DrawDesc.PSODesc.MemberName;
    #define COPY_PASS_DESC_MEMBER(MemberName) Desc.MemberName = PSOInitializer.PassDesc.MemberName;

    COPY_PASS_DESC_MEMBER(StreamOutput)
    COPY_DRAW_DESC_MEMBER(BlendState)
    COPY_PASS_DESC_MEMBER(SampleMask)
    COPY_DRAW_DESC_MEMBER(RasterizerState)
    COPY_DRAW_DESC_MEMBER(DepthStencilState)
    COPY_DRAW_DESC_MEMBER(InputLayout)
    COPY_PASS_DESC_MEMBER(IBStripCutValue)
    COPY_PASS_DESC_MEMBER(PrimitiveTopologyType)
    COPY_PASS_DESC_MEMBER(NumRenderTargets)
    EA::StdC::Memcpy(&(Desc.RTVFormats), &(PSOInitializer.PassDesc.RTVFormats), sizeof(DXGI_FORMAT) * ARRAY_LENGTH(PSOInitializer.PassDesc.RTVFormats));
    COPY_PASS_DESC_MEMBER(DSVFormat)
    COPY_PASS_DESC_MEMBER(SampleDesc)
    COPY_PASS_DESC_MEMBER(NodeMask)
    COPY_PASS_DESC_MEMBER(CachedPSO)
    COPY_PASS_DESC_MEMBER(Flags)

    // error fix #1
    // D3D12 ERROR : ID3D12Device::CreateInputLayout : Input Signature in bytecode could not be parsed.Data may be corrupt or in an unrecognizable format.[STATE_CREATION ERROR #63: CREATEINPUTLAYOUT_UNPARSEABLEINPUTSIGNATURE]
    // D3D12 ERROR : ID3D12Device::CreateVertexShader : Vertex Shader is corrupt or in an unrecognized format.[STATE_CREATION ERROR #67: CREATEVERTEXSHADER_INVALIDSHADERBYTECODE]
    // D3D12 ERROR : ID3D12Device::CreatePixelShader : Pixel Shader is corrupt or in an unrecognized format.[STATE_CREATION ERROR #93: CREATEPIXELSHADER_INVALIDSHADERBYTECODE]
    // 
    // This is caused by the compiler creating unsigned bytecode. "dxil.dll" must be put in the same folder as "dxcompiler.dll" at runtime
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

void FD3D12PSOManager::OnStartFrame(FD3D12CommandContext& InCommandContext)
{

}

void FD3D12PSOManager::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}
