#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Shader.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12RootSignature;

struct FD3D12PSOInitializer
{
	FBoundShaderSet BoundShaderSet;

    struct FDesc
    {
        D3D12_STREAM_OUTPUT_DESC StreamOutput;
        D3D12_BLEND_DESC BlendState;
        UINT SampleMask;
        D3D12_RASTERIZER_DESC RasterizerState;
        D3D12_DEPTH_STENCIL_DESC DepthStencilState;
        D3D12_INPUT_LAYOUT_DESC InputLayout;
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
        UINT NumRenderTargets;
        DXGI_FORMAT RTVFormats[8];
        DXGI_FORMAT DSVFormat;
        DXGI_SAMPLE_DESC SampleDesc;
        UINT NodeMask;
        D3D12_CACHED_PIPELINE_STATE CachedPSO;
        D3D12_PIPELINE_STATE_FLAGS Flags;
    } Desc;

	uint64 CachedHash;

    bool IsValid() const;
    void Reset();
    void FinishCreating();
};

inline bool operator==(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return lhs.CachedHash == rhs.CachedHash;
}
inline bool operator!=(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return lhs.CachedHash != rhs.CachedHash;
}

struct FD3D12PSO
{
    FD3D12PSOInitializer PSOInitializer;
	ComPtr<ID3D12PipelineState> PSOObject;

    FD3D12PSO() = delete;
    FD3D12PSO(const FD3D12PSOInitializer& InPSOInitializer);
};

namespace eastl
{
    template <> struct hash<FD3D12PSOInitializer>
    {
        size_t operator()(FD3D12PSOInitializer val) const { EA_ASSERT(val.CachedHash != 0); return static_cast<size_t>(val.CachedHash); }
    };

    template <> struct less<FD3D12PSOInitializer>
    {
        EA_CPP14_CONSTEXPR bool operator()(const FD3D12PSOInitializer& a, const FD3D12PSOInitializer& b) const
        {
            return a.CachedHash < b.CachedHash;
        }
    };
}

class FD3D12PSOManager : public EA::StdC::Singleton<FD3D12PSOManager>, public ID3D12RendererStateCallbackInterface
{
public:

    FD3D12PSO* GetOrCreatePSO(const FD3D12PSOInitializer& InD3D12PSOInitializer);

    virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
    virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

private:

    eastl::vector_map<FD3D12PSOInitializer, eastl::unique_ptr<FD3D12PSO>> PSOHashMap;

};

