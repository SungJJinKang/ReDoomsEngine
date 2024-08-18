#pragma once
#include "D3D12Include.h"
#include "D3D12PSO.h"

struct FD3D12CommandContext;
class FBoundShaderSet;
class FD3D12View;
class FD3D12DescriptorHeap;
class FD3D12RootSignature;
class FD3D12ShaderResourceView;

class FD3D12StateCache
{
public:

	void SetPSO(const FD3D12PSOInitializer& InPSOInitializer);
	void SetPSODrawDesc(const FD3D12PSOInitializer::FDrawDesc& InDrawDesc);
	void SetPSOPassDesc(const FD3D12PSOInitializer::FPassDesc& InPassDesc);
	void SetPSOInputLayout(const D3D12_INPUT_LAYOUT_DESC& InputLayoutDesc);
	void SetRasterizeDesc(const CD3DX12_RASTERIZER_DESC& RasterizeDesc);
	void SetBlendDesc(const CD3DX12_BLEND_DESC& BlendDesc);
	void SetDepthEnable(const bool bInEnable);
	void SetStencilEnable(const bool bInEnable);
	void SetBoundShaderSet(const FBoundShaderSet& InBoundShaderSet);
	void SetRenderTargets(const eastl::array<FD3D12Texture2DResource*, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>& InRenderTargets);
	void SetDepthStencilTarget(FD3D12Texture2DResource* const InDepthStencilTarget);
	void SetSRVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_SRVS>& BindPointInfos);
	void SetUAVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_UAVS>& BindPointInfos);
	void SetConstantBuffer(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV>& BindPointInfos);

	void SetVertexBufferViewList(const eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW>& InVertexBufferViewList);
	void SetIndexBufferView(const D3D12_INDEX_BUFFER_VIEW InIndexBufferView);

	void Flush(FD3D12CommandContext& InCommandList, const EPipeline InPipeline);
	void ResetForNewCommandlist();
	void ResetToDefault();

private:

	void SetRootSignature(FD3D12RootSignature* const InRootSignature);

	void ApplyPSO(FD3D12CommandList& InCommandList);
	void ApplyPrimitiveTopologyDirty(FD3D12CommandList& InCommandList);
	void ApplyRTVAndDSV(FD3D12CommandList& InCommandList);
	void ApplyRootSignature(FD3D12CommandList& InCommandList);
	void ApplyDescriptorHeap(FD3D12CommandList& InCommandList);
	void ApplySRVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplyUAVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplyConstantBuffers(FD3D12CommandList& InCommandList);
	void ApplyVertexBufferViewList(FD3D12CommandList& InCommandList);
	void ApplyIndexBufferView(FD3D12CommandList& InCommandList);

	bool bIsPSODirty = true;
	FD3D12PSOInitializer CachedPSOInitializer{};

	bool bIsPrimitiveTopologyDirty = true;

	bool bIsRootSignatureDirty = true;
	FD3D12RootSignature* CachedRootSignature = nullptr;

	struct FBoundResourceViewInfo
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle{};
		const FD3D12View* ResourceView = nullptr;
	};

	eastl::bitset<EShaderFrequency::NumShaderFrequency> DirtyFlagsOfSRVs;
	eastl::array<eastl::array<FBoundResourceViewInfo, MAX_SRVS>, EShaderFrequency::NumShaderFrequency> CachedSRVs;

	eastl::bitset<EShaderFrequency::NumShaderFrequency> DirtyFlagsOfUAVs;
	eastl::array<eastl::array<FBoundResourceViewInfo, MAX_UAVS>, EShaderFrequency::NumShaderFrequency> CachedUAVs;

// 	bool bIsRTVDirty = true;
// 	eastl::array<eastl::array<FViewBindPointInfo, MAX_SRVS>, EShaderFrequency::NumShaderFrequency> CachedRTVBindPointInfosOfFrequencies;
// 
// 	bool bIsDSVDirty = true;
// 	eastl::array<eastl::array<FViewBindPointInfo, MAX_SRVS>, EShaderFrequency::NumShaderFrequency> CachedDSVBindPointInfosOfFrequencies;

	// Constant buffer is bind only by root constant buffer view
	bool bIsRootCBVDirty = true;
	eastl::array<eastl::array<D3D12_GPU_VIRTUAL_ADDRESS, MAX_ROOT_CBV>, EShaderFrequency::NumShaderFrequency> CachedConstantBufferGPUVirtualAddressOfFrequencies;
	eastl::array<eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV>, EShaderFrequency::NumShaderFrequency> CachedConstantBufferBindPointInfosOfFrequencies;

	bool bNeedToSetDescriptorHeaps = true;

	bool bNeedToSetRTVAndDSV = true;

	uint32_t CachedRTVCount = UINT32_MAX;
	eastl::array<FBoundResourceViewInfo, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> CachedRTVCPUHandleList;
	
	FBoundResourceViewInfo CachedDSVCPUHandle;

	eastl::fixed_vector<D3D12_VERTEX_BUFFER_VIEW, MAX_BOUND_VERTEX_BUFFER_VIEW> CachedVertexBufferViewList;
	bool bNeedToSetVertexBufferView = true;

	D3D12_INDEX_BUFFER_VIEW CachedIndexBufferView;
	bool bNeedToSetIndexBufferView = true;
};

