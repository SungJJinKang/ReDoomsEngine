#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "EASTL/queue.h"

class FD3D12DescriptorHeap;
class FD3D12Descriptor
{
};

struct FD3D12DescriptorHeapBlock
{
	FD3D12DescriptorHeapBlock() = default;
	FD3D12DescriptorHeapBlock(FD3D12DescriptorHeap* const InParentDescriptorHeap, int32_t InBaseSlot, uint32 InDescriptorSlotCount, uint32 InUsedDescriptorSlotCount);

	void Clear();

	FD3D12DescriptorHeap* ParentDescriptorHeap = nullptr;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle();

	int32_t BaseSlot = 0;
	uint32 DescriptorSlotCount = 0;
	uint32 UsedDescriptorSlotCount = 0; // assume slot is used from first to last
};

class FD3D12DescriptorHeap
{
public:

	FD3D12DescriptorHeap() = delete;
	FD3D12DescriptorHeap(uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	void Init();
	
	bool IsShaderVisible() const
	{
		return HeapFlags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	uint32_t GetDescriptorSize() const 
	{
		return DescriptorSize;
	}

	void MakeFreed();

	bool AllocateFreeDescriptorHeapBlock(FD3D12DescriptorHeapBlock& bOutDescriptorHeapBlock, const uint32 InDescriptorCount);
	void FreeDescriptorHeapBlock(FD3D12DescriptorHeapBlock& InHeapBlock);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUBase() const
	{
		EA_ASSERT(CPUBase.ptr);
		return CPUBase;
	}
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUBase() const
	{
		EA_ASSERT(IsShaderVisible());
		EA_ASSERT(GPUBase.ptr);
		return GPUBase;
	}

	ComPtr<ID3D12DescriptorHeap> D3DDescriptorHeap;

private:

	uint32_t NumDescriptors;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlags;
	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	uint32_t DescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUBase;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUBase;

	eastl::vector<FD3D12DescriptorHeapBlock> FreeDescriptorHeapBlockList;;
};

class FD3D12DescriptorHeapContainer
{
public:

	FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, const uint32_t InNumDescriptor);
	virtual void Init();
	virtual bool IsOnlineHeap() const = 0;
	bool IsOfflineHeap() const
	{
		return !IsOnlineHeap();
	}

	// this function never fail
	FD3D12DescriptorHeapBlock AllocateDescriptorHeapBlock(const uint32 InDescriptorCount);

protected:

	virtual FD3D12DescriptorHeap* AllocateNewHeap();
	void FreeNewHeap(FD3D12DescriptorHeap* const InHeap);

	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlag;
	uint32_t NumDescriptor;

	eastl::vector<eastl::unique_ptr<FD3D12DescriptorHeap>> DescriptorHeapListAllocatedToUser;
	eastl::queue<eastl::unique_ptr<FD3D12DescriptorHeap>> FreeDescriptorHeapList;

};

/// <summary>
/// Manage shader visible descriotpr heap
/// Have a large heap. If the heap is completely exhausted, we just assert it.
/// Why use online/offline heap? Changing set descriptor heap flushes gpu pipeline
/// </summary>
class FD3D12OnlineDescriptorHeapContainer : public FD3D12DescriptorHeapContainer
{
public:

	FD3D12OnlineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	virtual void Init();

	virtual bool IsOnlineHeap() const
	{
		return true;
	}

private:

};



class FD3D12OfflineDescriptorHeapContainer : public FD3D12DescriptorHeapContainer
{
public:

	FD3D12OfflineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	virtual void Init();

	virtual bool IsOnlineHeap() const
	{
		return false;
	}

private:

};

class FD3D12DescriptorHeapManager : public EA::StdC::Singleton<FD3D12DescriptorHeapManager>
{
public:

	FD3D12DescriptorHeapManager();
	void Init();

	FD3D12DescriptorHeapContainer* GetOnlineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	FD3D12DescriptorHeapContainer* GetOfflineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);

	FD3D12DescriptorHeapBlock AllocateOnlineHeapDescriptorHeapBlock(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const uint32 InDescriptorCount);
	FD3D12DescriptorHeapBlock AllocateOfflineHeapDescriptorHeapBlock(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const uint32 InDescriptorCount);

private:

	FD3D12OfflineDescriptorHeapContainer RTVDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer DSVDescriptorHeapContainer;
	FD3D12OnlineDescriptorHeapContainer CbvSrvUavOnlineDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer CbvSrvUavOfflineDescriptorHeapContainer;
	FD3D12OnlineDescriptorHeapContainer SamplerOnlineDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer SamplerOfflineDescriptorHeapContainer;
};
