#pragma once
#include "D3D12PSO.h"

class FD3D12DescriptorHeap;

struct FD3D12ResourceCache
{
	eastl::bitvector<> DirtyFlags[D3D12_SHADER_VISIBILITY_NUM];
};

struct FD3D12ResourceViewCache : public FD3D12ResourceCache
{

};

class FD3D12StateCache
{
public:

	void Flush();

private:

	FD3D12PSOInitializer PSOInitializer;
	FD3D12DescriptorHeap* ViewHeap;
	FD3D12DescriptorHeap* SamplerHeap;

};

