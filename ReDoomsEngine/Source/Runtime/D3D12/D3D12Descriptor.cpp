#include "D3D12Descriptor.h"

#include "D3D12Device.h"

static uint32_t GetNumDescriptorsForOfflineHeap(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
{
    switch (InHeapType)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:    return 2048;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:            return 256;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:            return 256;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:        return 128;
    default: EA_ASSUME(0);
    }
}

// This value defines how many descriptors will be in the device online view heap which
// is shared across contexts to allow the driver to eliminate redundant descriptor heap sets.
// This should be tweaked for each title as heaps require VRAM. The default value of 512k takes up ~16MB
static uint32_t GOnlineDescriptorHeapSize = 500 * 1000;

FD3D12DescriptorHeap::FD3D12DescriptorHeap(uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
	: NumDescriptors(InNumDescriptors), HeapFlags(InHeapFlags), HeapType(InHeapType), DescriptorSize(), CPUBase(), GPUBase(), FreeSlot()
{
    EA_ASSERT(NumDescriptors > 0);
}

void FD3D12DescriptorHeap::Init()
{
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
    MEM_ZERO(DescriptorHeapDesc);
    DescriptorHeapDesc.NumDescriptors = NumDescriptors;
    DescriptorHeapDesc.Type = HeapType;
    DescriptorHeapDesc.Flags = HeapFlags;
    VERIFYD3D12RESULT(GetD3D12Device()->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3DDescriptorHeap)));

    DescriptorSize = GetD3D12Device()->GetDescriptorHandleIncrementSize(HeapType);

    CPUBase = D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    GPUBase = D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

    FreeSlot.set(NumDescriptors, true);
}

FD3D12DescriptorHeapContainer::FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, const uint32_t InNumDescriptor)
    : HeapType(InHeapType), HeapFlag(InHeapFlags), NumDescriptor(InNumDescriptor)
{
}

void FD3D12DescriptorHeapContainer::Init()
{
}

eastl::unique_ptr<FD3D12DescriptorHeap>& FD3D12DescriptorHeapContainer::AllocateNewHeap()
{
    eastl::unique_ptr<FD3D12DescriptorHeap>& OfflineHeap =
        DescriptorHeapList.emplace_back(eastl::make_unique<FD3D12DescriptorHeap>(NumDescriptor, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, HeapType));
    OfflineHeap->Init();
    return OfflineHeap;
}

FD3D12OnlineDescriptorHeapContainer::FD3D12OnlineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
    : FD3D12DescriptorHeapContainer(InHeapType, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, GOnlineDescriptorHeapSize)
{
}

void FD3D12OnlineDescriptorHeapContainer::Init()
{
    FD3D12DescriptorHeapContainer::Init();
    AllocateNewHeap();
}

FD3D12OfflineDescriptorHeapContainer::FD3D12OfflineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
    : FD3D12DescriptorHeapContainer(InHeapType, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, GetNumDescriptorsForOfflineHeap(InHeapType))
{
}

void FD3D12OfflineDescriptorHeapContainer::Init()
{
    FD3D12DescriptorHeapContainer::Init();
    AllocateNewHeap();
}

FD3D12DescriptorHeapManager::FD3D12DescriptorHeapManager()
    :
    RTVDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
    DSVDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_DSV),
    CbvSrvUavOnlineDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
    CbvSrvUavOfflineDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
    SamplerOnlineDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER),
    SamplerOfflineDescriptorHeapContainer(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
{
}

void FD3D12DescriptorHeapManager::Init()
{
    RTVDescriptorHeapContainer.Init();
    DSVDescriptorHeapContainer.Init();
    CbvSrvUavOnlineDescriptorHeapContainer.Init();
    CbvSrvUavOfflineDescriptorHeapContainer.Init();
    SamplerOnlineDescriptorHeapContainer.Init();
    SamplerOfflineDescriptorHeapContainer.Init();
}
