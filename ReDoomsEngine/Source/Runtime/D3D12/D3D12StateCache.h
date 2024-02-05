#pragma once
#include "D3D12PSO.h"

struct FD3D12CommandContext;
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

	struct FConstantBufferBindPointInfo
	{
		FD3D12ConstantBufferResource* ConstantBufferResource;
		const FD3D12ConstantBufferReflectionData* ReflectionData;
	};

	void Reset();
	void ApplyConstantBuffer(FD3D12CommandContext& const InCommandContext, const EShaderFrequency InShaderFrequency, const FD3D12RootSignature* const InRootSignature, const eastl::vector<FConstantBufferBindPointInfo>& BindPoints);
	void Flush();

private:

	FD3D12PSOInitializer PSOInitializer;
	FD3D12DescriptorHeap* ViewHeap;
	FD3D12DescriptorHeap* SamplerHeap;

};

