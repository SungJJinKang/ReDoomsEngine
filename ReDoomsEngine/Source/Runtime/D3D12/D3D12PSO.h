#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Shader.h"
#include "Common/RendererStateCallbackInterface.h"

class FD3D12RootSignature;

class FD3D12PSOInitializer
{
public:
    /// <summary>
    /// This struct should contains states changed frequently per draw
    /// </summary>
    mutable struct FDrawDesc
    {
        FBoundShaderSet BoundShaderSet;

        struct FDesc
        {
			D3D12_INPUT_LAYOUT_DESC InputLayout{};
			D3D12_CACHED_PIPELINE_STATE CachedPSO{};
			D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

            bool operator==(const FDesc& rhs) const
            {
                return InputLayout == rhs.InputLayout &&
                       CachedPSO == rhs.CachedPSO &&
                       PrimitiveTopologyType == rhs.PrimitiveTopologyType;
            }
            bool operator!=(const FDesc& rhs) const
            {
                return !(*this == rhs);
            }
        } Desc;
        uint64 CachedDescHash = 0;
        void CacheDescHash();

        inline bool IsValidHash() const
        {
            return BoundShaderSet.IsValidHash() && (CachedDescHash != 0);
        }

		inline bool operator==(const FDrawDesc& rhs) const
        {
			return BoundShaderSet == rhs.BoundShaderSet && Desc == rhs.Desc;
        }
		inline bool operator!=(const FDrawDesc& rhs) const
		{
			return !(*this == rhs);
		}
    } DrawDesc;

    /// <summary>
    /// This struct should contains states never changed during a pass
    /// </summary>
	mutable struct FPassDesc
    {
        struct FDesc
        {
            D3D12_STREAM_OUTPUT_DESC StreamOutput;
            UINT SampleMask;
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
            UINT NumRenderTargets = 0;
			DXGI_FORMAT RTVFormats[8]{ DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN };
            DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN;
            DXGI_SAMPLE_DESC SampleDesc;
            UINT NodeMask;
			D3D12_PIPELINE_STATE_FLAGS Flags;
			CD3DX12_BLEND_DESC BlendState{ D3D12_DEFAULT };
			CD3DX12_RASTERIZER_DESC RasterizerState{ D3D12_DEFAULT };
			CD3DX12_DEPTH_STENCIL_DESC DepthStencilState{ D3D12_DEFAULT };

			inline bool operator==(const FDesc& rhs) const
			{
				return StreamOutput == rhs.StreamOutput &&
					   SampleMask == rhs.SampleMask &&
					   IBStripCutValue == rhs.IBStripCutValue &&
					   NumRenderTargets == rhs.NumRenderTargets &&
					   DSVFormat == rhs.DSVFormat &&
					   SampleDesc == rhs.SampleDesc &&
					   NodeMask == rhs.NodeMask &&
					   Flags == rhs.Flags &&
					   BlendState == rhs.BlendState &&
					   RasterizerState == rhs.RasterizerState &&
					   DepthStencilState == rhs.DepthStencilState;
			}
			inline bool operator!=(const FDesc& rhs) const
			{
				return !(*this == rhs);
			}
        } Desc;

        uint64 CachedDescHash = 0;
        void CacheDescHash();
        inline bool IsValidHash() const
        {
            return (CachedDescHash != 0);
        }

		inline bool operator==(const FPassDesc& rhs) const
		{
			return Desc == rhs.Desc;
		}

		inline bool operator!=(const FPassDesc& rhs) const
		{
			return !(*this == rhs);
		}
    } PassDesc;

    inline uint64 GetCachedHash() const
    {
		if (!IsValidHash())
		{
			CacheHash();
		}
        return CachedHash;
    }

	inline bool IsValidHash() const
	{
        return (CachedHash != 0) && DrawDesc.IsValidHash() && PassDesc.IsValidHash();
	}

	bool IsValid() const;

    //void Reset();
    void CacheHash() const;
    
private:

	mutable uint64 CachedHash;
};

inline bool operator==(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return (lhs.DrawDesc == rhs.DrawDesc) && (lhs.PassDesc == rhs.PassDesc);
}
inline bool operator!=(const FD3D12PSOInitializer& lhs, const FD3D12PSOInitializer& rhs)
{
    return !(lhs == rhs);
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

class FD3D12PSOManager : public EA::StdC::Singleton<FD3D12PSOManager>, public IRendererStateCallbackInterface
{
public:

    FD3D12PSO* GetOrCreatePSO(const FD3D12PSOInitializer& InD3D12PSOInitializer);

    virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
    virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

private:

    eastl::vector_map<FD3D12PSOInitializer, eastl::unique_ptr<FD3D12PSO>> PSOHashMap;

};

