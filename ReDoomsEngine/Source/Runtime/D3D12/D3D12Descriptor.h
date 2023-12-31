#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Descriptor
{
};

class FD3D12DescriptorHeap
{
public:

	FD3D12DescriptorHeap() = delete;
	FD3D12DescriptorHeap(uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	void Init();
	ID3D12DescriptorHeap* GetD3DDescriptorHeap() const
	{
		EA_ASSERT(D3DDescriptorHeap.Get());
		return D3DDescriptorHeap.Get();
	}
	
	bool IsShaderVisible() const
	{
		return HeapFlags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUBase() const
	{
		EA_ASSERT(CPUBase.ptr);
		return CPUBase;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUBase() const
	{
		EA_ASSERT(IsShaderVisible());
		EA_ASSERT(GPUBase.ptr);
		return GPUBase;
	}

private:

	ComPtr<ID3D12DescriptorHeap> D3DDescriptorHeap;

	uint32_t NumDescriptors;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlags;
	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	uint32_t DescriptorSize;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUBase;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUBase;

	eastl::bitvector<> FreeSlot;
};

class FD3D12DescriptorHeapContainer
{
public:

	FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, const uint32_t InNumDescriptor);
	virtual void Init();

protected:

	eastl::unique_ptr<FD3D12DescriptorHeap>& AllocateNewHeap();

	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	D3D12_DESCRIPTOR_HEAP_FLAGS HeapFlag;
	uint32_t NumDescriptor;
	eastl::vector<eastl::unique_ptr<FD3D12DescriptorHeap>> DescriptorHeapList;

};

/// <summary>
/// Manage shader visible descriotpr heap
/// Have a large heap. If the heap is completely exhausted, we just assert it.
/// </summary>
class FD3D12OnlineDescriptorHeapContainer : public FD3D12DescriptorHeapContainer
{
public:

	FD3D12OnlineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType);
	virtual void Init();

private:

	/// <summary>
	/// Large Heap. Shader Visible Heap
	/// </summary>
	eastl::unique_ptr<FD3D12DescriptorHeap> DescriptorHeap;
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
