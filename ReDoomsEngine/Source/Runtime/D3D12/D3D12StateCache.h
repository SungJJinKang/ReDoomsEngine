#pragma once
#include "D3D12Include.h"
#include "D3D12PSO.h"

struct FD3D12CommandContext;
class FD3D12View;
class FD3D12DescriptorHeap;
class FD3D12RootSignature;
class FD3D12ShaderResourceView;

class FD3D12StateCache
{
public:

	void SetPSO(const FD3D12PSOInitializer& InPSOInitializer);
	void SetSRVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_SRVS>& BindPointInfos);
	void SetUAVs(const EShaderFrequency InShaderFrequency, const eastl::array<FD3D12ShaderResourceView*, MAX_UAVS>& BindPointInfos);
	void SetConstantBuffer(const EShaderFrequency InShaderFrequency, const eastl::array<FShaderParameterConstantBuffer*, MAX_ROOT_CBV>& BindPointInfos);
	void Flush(FD3D12CommandContext& InCommandList);
	void ResetForNewCommandlist();

private:

	void SetRootSignature(FD3D12RootSignature* const InRootSignature);

	void ApplyPSO(FD3D12CommandList& InCommandList);
	void ApplyRootSignature(FD3D12CommandList& InCommandList);
	void ApplyDescriptorHeap(FD3D12CommandList& InCommandList);
	void ApplySRVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplyUAVs(FD3D12CommandList& InCommandList, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplyConstantBuffers(FD3D12CommandList& InCommandList);

	bool bIsPSODirty = true;
	FD3D12PSOInitializer CachedPSOInitializer{};

	bool bIsRootSignatureDirty = true;
	FD3D12RootSignature* CachedRootSignature = nullptr;

	eastl::bitset<EShaderFrequency::NumShaderFrequency> DirtyFlagsOfSRVs;
	eastl::array<eastl::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, MAX_SRVS>, EShaderFrequency::NumShaderFrequency> CachedSRVs;

	eastl::bitset<EShaderFrequency::NumShaderFrequency> DirtyFlagsOfUAVs;
	eastl::array<eastl::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, MAX_UAVS>, EShaderFrequency::NumShaderFrequency> CachedUAVs;

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
	eastl::shared_ptr<FD3D12DescriptorHeap> CachedSrvUavOnlineDescriptorHeap{};
};

