#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Shader.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12RootSignature;

class FD3D12PSOInitializer
{
public:
    /// <summary>
    /// This struct should contains states changed frequently per draw
    /// </summary>
    struct FDrawDesc
    {
        FBoundShaderSet BoundShaderSet;

        struct FDesc
        {
            D3D12_INPUT_LAYOUT_DESC InputLayout;
            CD3DX12_BLEND_DESC BlendState;
            CD3DX12_RASTERIZER_DESC RasterizerState;
            CD3DX12_DEPTH_STENCIL_DESC DepthStencilState;
            D3D12_CACHED_PIPELINE_STATE CachedPSO;
            D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
        } Desc;
        uint64 CachedDescHash = 0;
        void CacheDescHash();

        inline bool IsValidHash() const
        {
            return BoundShaderSet.IsValidHash() && (CachedDescHash != 0);
        }
    } DrawDesc;

    /// <summary>
    /// This struct should contains states never changed during a pass
    /// </summary>
    struct FPassDesc
    {
        struct FDesc
        {
            D3D12_STREAM_OUTPUT_DESC StreamOutput;
            UINT SampleMask;
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
            UINT NumRenderTargets;
            DXGI_FORMAT RTVFormats[8];
            DXGI_FORMAT DSVFormat;
            DXGI_SAMPLE_DESC SampleDesc;
            UINT NodeMask;
            D3D12_PIPELINE_STATE_FLAGS Flags;
        } Desc;
        uint64 CachedDescHash = 0;
        void CacheDescHash();
        inline bool IsValidHash() const
        {
            return (CachedDescHash != 0);
        }
    } PassDesc;

    inline uint64 GetCachedHash() const
    {
        EA_ASSERT(IsValidHash());
        return CachedHash;
    }

	inline bool IsValidHash() const
	{
        return (CachedHash != 0) && DrawDesc.IsValidHash() && PassDesc.IsValidHash();
	}

    //void Reset();
    void CacheHash();
    
private:

	uint64 CachedHash;
};

inline bool operator==(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return (lhs.GetCachedHash() == rhs.GetCachedHash());
}
inline bool operator!=(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return (lhs.GetCachedHash() != rhs.GetCachedHash());
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
        size_t operator()(FD3D12PSOInitializer val) const { EA_ASSERT(val.GetCachedHash() != 0); return static_cast<size_t>(val.GetCachedHash()); }
    };

    template <> struct less<FD3D12PSOInitializer>
    {
        bool operator()(const FD3D12PSOInitializer& a, const FD3D12PSOInitializer& b) const
        {
            return a.GetCachedHash() < b.GetCachedHash();
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

