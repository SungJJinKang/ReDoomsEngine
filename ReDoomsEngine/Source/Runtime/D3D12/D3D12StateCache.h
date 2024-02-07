#pragma once
#include "D3D12Include.h"
#include "D3D12PSO.h"

struct FD3D12CommandContext;
class FD3D12View;
class FD3D12DescriptorHeap;
class FD3D12RootSignature;

struct FD3D12ResourceCache
{
	eastl::bitvector<> DirtyFlags[EShaderFrequency::NumShaderFrequency];
};

struct FD3D12ResourceViewCache : public FD3D12ResourceCache
{

};

class FD3D12StateCache
{
public:

	struct FViewBindPointInfo
	{
		FD3D12View* ResourceView;
		D3D12_SHADER_INPUT_BIND_DESC InputBindDesc;
	};

	struct FConstantBufferBindPointInfo
	{
		FD3D12ConstantBufferResource* ConstantBufferResource;
		const FD3D12ConstantBufferReflectionData* ReflectionData;
	};

	void SetSRVs(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FViewBindPointInfo>& BindPointInfos);
	void SetUAVs(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FViewBindPointInfo>& BindPointInfos);
	void SetConstantBuffer(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FConstantBufferBindPointInfo>& BindPointInfos);
	void Flush(FD3D12CommandContext& const InCommandContext);
	void Reset();
	void SetDescriptorHeaps(FD3D12CommandContext& const InCommandContext, eastl::vector<eastl::shared_ptr<FD3D12DescriptorHeap>> InDescriptorHeaps);

private:

	void SetTargetRootSignature(const FD3D12RootSignature* const InRootSignature);
	void ApplyUAVs(FD3D12CommandContext& const InCommandContext, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplySRVs(FD3D12CommandContext& const InCommandContext, const FD3D12DescriptorHeapBlock& BaseHeapBlcok, uint32_t& OutUsedBlockCount);
	void ApplyConstantBuffers(FD3D12CommandContext& const InCommandContext);

	FD3D12PSOInitializer PSOInitializer{};
	const FD3D12RootSignature* TargetRootSignature = nullptr;

	uint32_t CurrentCbvSrvUavSlotIndex = 0;

	eastl::array<eastl::vector<FViewBindPointInfo>, EShaderFrequency::NumShaderFrequency> CachedSRVBindPointInfosOfFrequencies;
	eastl::array<eastl::vector<FViewBindPointInfo>, EShaderFrequency::NumShaderFrequency> CachedUAVBindPointInfosOfFrequencies;
	eastl::array<eastl::vector<FViewBindPointInfo>, EShaderFrequency::NumShaderFrequency> CachedRTVBindPointInfosOfFrequencies;
	eastl::array<eastl::vector<FViewBindPointInfo>, EShaderFrequency::NumShaderFrequency> CachedDSVBindPointInfosOfFrequencies;

	// Constant buffer is bind only by root constant buffer view
	eastl::array<eastl::vector<FConstantBufferBindPointInfo>, EShaderFrequency::NumShaderFrequency> CachedConstantBufferBindPointInfosOfFrequencies;

	eastl::vector<eastl::shared_ptr<FD3D12DescriptorHeap>> CachedSetDescriptorHeaps{};
};

