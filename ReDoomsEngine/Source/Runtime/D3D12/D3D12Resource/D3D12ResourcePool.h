#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Fence.h"

class FD3D12Resource;
struct FD3D12ResourcePoolHeapContainer;
struct FD3D12ResourcePoolBlock
{
	eastl::weak_ptr<FD3D12ResourcePoolHeapContainer> OwnerResourcePoolHeapContainer;

	uint64_t Size;
	uint64_t OffsetFromBase;

	void FreeBlock();
};


struct FD3D12ResourcePoolHeapContainer
{
	ComPtr<ID3D12Heap> Heap;

	eastl::vector<FD3D12ResourcePoolBlock> AllocatedBlockList;
	eastl::vector<FD3D12ResourcePoolBlock> FreeBlockList;
};

struct FD3D12ResourcePool
{
	enum EResourcePoolType
	{
		ReadOnlyTexture,
		RenderTarget,
		UAVTexture,

		Num
	};

	EResourcePoolType ResourcePoolType;
	eastl::vector<eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>> ResourcePoolHeapContainerList;
	D3D12_HEAP_DESC HeapDesc;

	bool IsForPlacedResource() const;

	bool AllocateBlock(const uint64_t InSize, const uint64_t InAlignment, FD3D12ResourcePoolBlock& OutAllocatedBlock);
};