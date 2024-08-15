#pragma once
#include "d3d12.h"
#include "CommonInclude.h"

inline bool operator==(const D3D12_VERTEX_BUFFER_VIEW& A,const D3D12_VERTEX_BUFFER_VIEW& B)
{
	return A.BufferLocation == B.BufferLocation
		&& A.SizeInBytes == B.SizeInBytes
		&& A.StrideInBytes == B.StrideInBytes;
}

inline bool operator!=(const D3D12_VERTEX_BUFFER_VIEW& A,const D3D12_VERTEX_BUFFER_VIEW& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_INDEX_BUFFER_VIEW& A,const D3D12_INDEX_BUFFER_VIEW& B)
{
	return A.BufferLocation == B.BufferLocation
		&& A.SizeInBytes == B.SizeInBytes
		&& A.Format == B.Format;
}

inline bool operator!=(const D3D12_INDEX_BUFFER_VIEW& A,const D3D12_INDEX_BUFFER_VIEW& B)
{
	return !(A == B);
}

namespace eastl
{
	template <> struct hash<D3D12_VERTEX_BUFFER_VIEW>
	{
		size_t operator()(D3D12_VERTEX_BUFFER_VIEW val) const { return CityHash64(reinterpret_cast<const char*>(&val), sizeof(val)); }
	};

	template <> struct hash<D3D12_INDEX_BUFFER_VIEW>
	{
		size_t operator()(D3D12_INDEX_BUFFER_VIEW val) const { return CityHash64(reinterpret_cast<const char*>(&val), sizeof(val)); }
	};
}

inline bool operator==(const D3D12_CACHED_PIPELINE_STATE& A, const D3D12_CACHED_PIPELINE_STATE& B)
{
	return A.CachedBlobSizeInBytes == B.CachedBlobSizeInBytes && A.pCachedBlob == B.pCachedBlob;
}

inline bool operator!=(const D3D12_CACHED_PIPELINE_STATE& A, const D3D12_CACHED_PIPELINE_STATE& B)
{
    return !(A == B);
}

inline bool operator==(const D3D12_INPUT_LAYOUT_DESC& A, const D3D12_INPUT_LAYOUT_DESC& B)
{
    if (A.NumElements != B.NumElements)
        return false;

    for (UINT i = 0; i < A.NumElements; i++)
    {
        if (A.pInputElementDescs[i].SemanticName != B.pInputElementDescs[i].SemanticName ||
            A.pInputElementDescs[i].SemanticIndex != B.pInputElementDescs[i].SemanticIndex ||
            A.pInputElementDescs[i].Format != B.pInputElementDescs[i].Format ||
            A.pInputElementDescs[i].InputSlot != B.pInputElementDescs[i].InputSlot ||
            A.pInputElementDescs[i].AlignedByteOffset != B.pInputElementDescs[i].AlignedByteOffset ||
            A.pInputElementDescs[i].InputSlotClass != B.pInputElementDescs[i].InputSlotClass ||
            A.pInputElementDescs[i].InstanceDataStepRate != B.pInputElementDescs[i].InstanceDataStepRate)
        {
            return false;
        }
    }

    return true;
}

inline bool operator!=(const D3D12_INPUT_LAYOUT_DESC& A, const D3D12_INPUT_LAYOUT_DESC& B)
{
    return !(A == B);
}

inline bool operator==(const D3D12_STREAM_OUTPUT_DESC& A, const D3D12_STREAM_OUTPUT_DESC& B)
{
	if (A.NumEntries != B.NumEntries || A.NumStrides != B.NumStrides || A.RasterizedStream != B.RasterizedStream)
		return false;

	for (UINT i = 0; i < A.NumEntries; i++)
	{
		if (A.pSODeclaration[i].Stream != B.pSODeclaration[i].Stream ||
			A.pSODeclaration[i].SemanticName != B.pSODeclaration[i].SemanticName ||
			A.pSODeclaration[i].SemanticIndex != B.pSODeclaration[i].SemanticIndex ||
			A.pSODeclaration[i].StartComponent != B.pSODeclaration[i].StartComponent ||
			A.pSODeclaration[i].ComponentCount != B.pSODeclaration[i].ComponentCount ||
			A.pSODeclaration[i].OutputSlot != B.pSODeclaration[i].OutputSlot)
		{
			return false;
		}
	}

	for (UINT i = 0; i < A.NumStrides; i++)
	{
		if (A.pBufferStrides[i] != B.pBufferStrides[i])
		{
			return false;
		}
	}

	return true;
}

inline bool operator!=(const D3D12_STREAM_OUTPUT_DESC& A, const D3D12_STREAM_OUTPUT_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const DXGI_SAMPLE_DESC& A, const DXGI_SAMPLE_DESC& B)
{
	return A.Count == B.Count && A.Quality == B.Quality;
}

inline bool operator!=(const DXGI_SAMPLE_DESC& A, const DXGI_SAMPLE_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_RENDER_TARGET_BLEND_DESC& A, const D3D12_RENDER_TARGET_BLEND_DESC& B)
{
	return A.BlendEnable == B.BlendEnable &&
		A.LogicOpEnable == B.LogicOpEnable &&
		A.SrcBlend == B.SrcBlend &&
		A.DestBlend == B.DestBlend &&
		A.BlendOp == B.BlendOp &&
		A.SrcBlendAlpha == B.SrcBlendAlpha &&
		A.DestBlendAlpha == B.DestBlendAlpha &&
		A.BlendOpAlpha == B.BlendOpAlpha &&
		A.LogicOp == B.LogicOp &&
		A.RenderTargetWriteMask == B.RenderTargetWriteMask;
}

inline bool operator!=(const D3D12_RENDER_TARGET_BLEND_DESC& A, const D3D12_RENDER_TARGET_BLEND_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_BLEND_DESC& A, const D3D12_BLEND_DESC& B)
{
	if (A.AlphaToCoverageEnable != B.AlphaToCoverageEnable || A.IndependentBlendEnable != B.IndependentBlendEnable)
		return false;

	for (UINT i = 0; i < 8; i++)
	{
		if (A.RenderTarget[i] != B.RenderTarget[i])
		{
			return false;
		}
	}

	return true;
}

inline bool operator!=(const D3D12_BLEND_DESC& A, const D3D12_BLEND_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_RASTERIZER_DESC& A, const D3D12_RASTERIZER_DESC& B)
{
	return A.FillMode == B.FillMode &&
		A.CullMode == B.CullMode &&
		A.FrontCounterClockwise == B.FrontCounterClockwise &&
		A.DepthBias == B.DepthBias &&
		A.DepthBiasClamp == B.DepthBiasClamp &&
		A.SlopeScaledDepthBias == B.SlopeScaledDepthBias &&
		A.DepthClipEnable == B.DepthClipEnable &&
		A.MultisampleEnable == B.MultisampleEnable &&
		A.AntialiasedLineEnable == B.AntialiasedLineEnable &&
		A.ForcedSampleCount == B.ForcedSampleCount &&
		A.ConservativeRaster == B.ConservativeRaster;
}

inline bool operator!=(const D3D12_RASTERIZER_DESC& A, const D3D12_RASTERIZER_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_DEPTH_STENCILOP_DESC& A, const D3D12_DEPTH_STENCILOP_DESC& B)
{
	return A.StencilFailOp == B.StencilFailOp &&
		A.StencilDepthFailOp == B.StencilDepthFailOp &&
		A.StencilPassOp == B.StencilPassOp &&
		A.StencilFunc == B.StencilFunc;
}

inline bool operator!=(const D3D12_DEPTH_STENCILOP_DESC& A, const D3D12_DEPTH_STENCILOP_DESC& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_DEPTH_STENCIL_DESC& A, const D3D12_DEPTH_STENCIL_DESC& B)
{
	return A.DepthEnable == B.DepthEnable &&
		A.DepthWriteMask == B.DepthWriteMask &&
		A.DepthFunc == B.DepthFunc &&
		A.StencilEnable == B.StencilEnable &&
		A.StencilReadMask == B.StencilReadMask &&
		A.StencilWriteMask == B.StencilWriteMask &&
		A.FrontFace == B.FrontFace &&
		A.BackFace == B.BackFace;
}

inline bool operator!=(const D3D12_DEPTH_STENCIL_DESC& A, const D3D12_DEPTH_STENCIL_DESC& B)
{
	return !(A == B);
}

