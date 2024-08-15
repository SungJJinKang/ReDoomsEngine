#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "EASTL/queue.h"
#include "Common/RendererStateCallbackInterface.h"

class FD3D12DescriptorHeap;
class FD3D12DescriptorHeapContainer;

struct FD3D12DescriptorHeapBlock
{
	FD3D12DescriptorHeapBlock() = default;
	FD3D12DescriptorHeapBlock(eastl::weak_ptr<FD3D12DescriptorHeap> InParentDescriptorHeap, const uint32_t InBaseSlot, const uint32_t InDescriptorSlotCount, const uint32_t InDescriptorSize);

	void Clear();

	eastl::weak_ptr<FD3D12DescriptorHeap> ParentDescriptorHeap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() const;

	uint32_t BaseSlot = 0;
	uint32_t DescriptorSlotCount = 0;
	uint32_t DescriptorSize;
};

class FD3D12DescriptorHeap : public eastl::enable_shared_from_this<FD3D12DescriptorHeap>
{
public:

	FD3D12DescriptorHeap() = delete;
	virtual ~FD3D12DescriptorHeap() = default;
	FD3D12DescriptorHeap(const FD3D12DescriptorHeapContainer* const InD3D12DescriptorHeapContainer, uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	void Init();
	
	virtual bool IsShaderVisible() const
	{
		return HeapFlags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	uint32_t GetDescriptorSize() const 
	{
		return DescriptorSize;
	}

	void MakeFreed();

	bool AllocateFreeDescriptorHeapBlock(FD3D12DescriptorHeapBlock& bOutDescriptorHeapBlock, const uint32 InDescriptorCount);
	void FreeDescriptorHeapBlock(const FD3D12DescriptorHeapBlock& InHeapBlock);

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

	const FD3D12DescriptorHeapContainer* const OwnerDescriptorHeapContainer;
	uint32_t NumDescriptors;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlags;
	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	uint32_t DescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUBase;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUBase;

	eastl::vector<FD3D12DescriptorHeapBlock> FreeDescriptorHeapBlockList;
};

class FD3D12DescriptorHeapContainer
{
public:

	FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const uint32_t InNumDescriptor);
	virtual void Init();
	virtual bool IsOnlineHeap() const = 0;
	bool IsOfflineHeap() const
	{
		return !IsOnlineHeap();
	}

protected:

	bool bInit;
	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	uint32_t NumDescriptor;

};

/// <summary>
/// Manage shader visible descriotpr heap
/// Have a large heap. If the heap is completely exhausted, we just assert it.
/// 
/// Why use online/offline heap? Changing set descriptor heap flushes gpu pipeline(https://stackoverflow.com/questions/45303008/dx12-descriptor-heaps-management)
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

	FD3D12DescriptorHeapBlock ReserveTransientDescriptorHeapBlock(const uint32_t InDescriptorCount);
	FD3D12DescriptorHeapBlock AllocatePermanentDescriptorHeapBlock(const uint32_t InDescriptorCount);
	void Reset();

	eastl::shared_ptr<FD3D12DescriptorHeap> GetOnlineHeap() const;

private:

	eastl::shared_ptr<FD3D12DescriptorHeap> OnlineHeap;
	uint32_t CurrentAllocatedTransientBlockCount;
	uint32_t CurrentAllocatedPermanentBlockCount;

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

	FD3D12DescriptorHeapBlock AllocateDescriptorHeapBlock(const uint32 InDescriptorCount);
	void FreeDescriptorHeapBlock(const FD3D12DescriptorHeapBlock& InFreedHeapBlock);

private:

	FD3D12DescriptorHeap* AllocateNewHeap();
	void FreeNewHeap(FD3D12DescriptorHeap* const InHeap);

	eastl::vector<eastl::shared_ptr<FD3D12DescriptorHeap>> DescriptorHeapListAllocatedToUser;
	eastl::queue<eastl::shared_ptr<FD3D12DescriptorHeap>> FreeDescriptorHeapList;
};

class FD3D12DescriptorHeapManager : public EA::StdC::Singleton<FD3D12DescriptorHeapManager>, public IRendererStateCallbackInterface
{
public:

	FD3D12DescriptorHeapManager();
	void Init();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

	FD3D12OnlineDescriptorHeapContainer CbvSrvUavOnlineDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer RTVDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer DSVDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer CbvSrvUavOfflineDescriptorHeapContainer;

private:
};
