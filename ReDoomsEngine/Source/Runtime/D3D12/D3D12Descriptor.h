#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "EASTL/queue.h"

class FD3D12Descriptor
{
};

struct FD3D12DescriptorHeapBlock
{

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

	eastl::bitvector<> FreeSlot;
};

struct FD3D12OfflineDescriptor
{
	FD3D12DescriptorHeap* ParentDescriptorHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUBase;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUBase;
};

class FD3D12DescriptorHeapContainer
{
public:

	FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, const uint32_t InNumDescriptor);
	virtual void Init();

	FD3D12DescriptorHeap* AllocateNewHeap();
	void FreeNewHeap(FD3D12DescriptorHeap* const InHeap);

protected:


	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlag;
	uint32_t NumDescriptor;
	eastl::queue<eastl::unique_ptr<FD3D12DescriptorHeap>> FreeDescriptorHeapList;
	eastl::vector<eastl::unique_ptr<FD3D12DescriptorHeap>> DescriptorHeapList;

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

private:

};



class FD3D12OfflineDescriptorHeapContainer : public FD3D12DescriptorHeapContainer
{
public:

	FD3D12OfflineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	virtual void Init();

private:

};

class FD3D12DescriptorHeapManager
{
public:

	FD3D12DescriptorHeapManager();
	void Init();

	FD3D12OfflineDescriptorHeapContainer RTVDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer DSVDescriptorHeapContainer;
	FD3D12OnlineDescriptorHeapContainer CbvSrvUavOnlineDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer CbvSrvUavOfflineDescriptorHeapContainer;
	FD3D12OnlineDescriptorHeapContainer SamplerOnlineDescriptorHeapContainer;
	FD3D12OfflineDescriptorHeapContainer SamplerOfflineDescriptorHeapContainer;
};
