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

FD3D12DescriptorHeapBlock::FD3D12DescriptorHeapBlock(FD3D12DescriptorHeap* const InParentDescriptorHeap, int32_t InBaseSlot, uint32 InDescriptorSlotCount, uint32 InUsedDescriptorSlotCount)
    : ParentDescriptorHeap(InParentDescriptorHeap), BaseSlot(InBaseSlot), DescriptorSlotCount(InDescriptorSlotCount), UsedDescriptorSlotCount(InUsedDescriptorSlotCount)
{

}

void FD3D12DescriptorHeapBlock::Clear()
{
    ParentDescriptorHeap = nullptr;
    BaseSlot = 0;
    DescriptorSlotCount = 0;
    UsedDescriptorSlotCount = 0;
}


CD3DX12_CPU_DESCRIPTOR_HANDLE FD3D12DescriptorHeapBlock::CPUDescriptorHandle()
{
    EA_ASSERT(DescriptorSlotCount > 0);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE{ ParentDescriptorHeap->GetCPUBase(), BaseSlot, ParentDescriptorHeap->GetDescriptorSize() };
}

CD3DX12_GPU_DESCRIPTOR_HANDLE FD3D12DescriptorHeapBlock::GPUDescriptorHandle()
{
    EA_ASSERT(DescriptorSlotCount > 0);
    return CD3DX12_GPU_DESCRIPTOR_HANDLE{ ParentDescriptorHeap->GetGPUBase(), BaseSlot, ParentDescriptorHeap->GetDescriptorSize() };
}

FD3D12DescriptorHeap::FD3D12DescriptorHeap(uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
	: NumDescriptors(InNumDescriptors), HeapFlags(InHeapFlags), HeapType(InHeapType), DescriptorSize(), CPUBase(), GPUBase()
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

    FD3D12DescriptorHeapBlock NewBlock{ this, 0, NumDescriptors * DescriptorSize, 0 };
    FreeDescriptorHeapBlock(NewBlock);
}

void FD3D12DescriptorHeap::MakeFreed()
{

}

bool FD3D12DescriptorHeap::AllocateFreeDescriptorHeapBlock(FD3D12DescriptorHeapBlock& bOutDescriptorHeapBlock, const uint32 InDescriptorCount)
{
    bool bSuccess = false;

    for (size_t FreeDescriptorHeapBlockIndex = 0; FreeDescriptorHeapBlockIndex < FreeDescriptorHeapBlockList.size(); ++FreeDescriptorHeapBlockIndex)
    {
        FD3D12DescriptorHeapBlock& FreeHeapBlock = FreeDescriptorHeapBlockList[FreeDescriptorHeapBlockIndex];
        EA_ASSERT(FreeHeapBlock.UsedDescriptorSlotCount == 0);

        if (FreeHeapBlock.DescriptorSlotCount >= InDescriptorCount)
        {
            bOutDescriptorHeapBlock = FreeHeapBlock;
            bOutDescriptorHeapBlock.UsedDescriptorSlotCount = InDescriptorCount;

            if (FreeHeapBlock.DescriptorSlotCount > InDescriptorCount)
            {
                FD3D12DescriptorHeapBlock NewlyFreedBlock{ this, FreeHeapBlock.BaseSlot +  static_cast<int32_t>(InDescriptorCount), FreeHeapBlock.DescriptorSlotCount - InDescriptorCount, 0 };
                FreeDescriptorHeapBlockList.emplace_back(NewlyFreedBlock);
            }

            FreeDescriptorHeapBlockList.erase(FreeDescriptorHeapBlockList.begin() + FreeDescriptorHeapBlockIndex);

            bSuccess = true;

            // @todo : merge with freed blocks
            break;
        }
    }
    
    return bSuccess;
}

void FD3D12DescriptorHeap::FreeDescriptorHeapBlock(FD3D12DescriptorHeapBlock& InHeapBlock)
{
    EA_ASSERT(InHeapBlock.UsedDescriptorSlotCount == 0);
    FreeDescriptorHeapBlockList.emplace_back(InHeapBlock);
}

FD3D12DescriptorHeapContainer::FD3D12DescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, const uint32_t InNumDescriptor)
    : HeapType(InHeapType), HeapFlag(InHeapFlags), NumDescriptor(InNumDescriptor)
{
}

void FD3D12DescriptorHeapContainer::Init()
{
}

FD3D12DescriptorHeapBlock FD3D12DescriptorHeapContainer::AllocateDescriptorHeapBlock(const uint32 InDescriptorCount)
{
    FD3D12DescriptorHeapBlock NewHeapBlock{};

    bool bSuccess = false;

    for (eastl::unique_ptr<FD3D12DescriptorHeap>& AllocatedDescriptorHeap : DescriptorHeapListAllocatedToUser)
    {
        if (AllocatedDescriptorHeap->AllocateFreeDescriptorHeapBlock(NewHeapBlock, InDescriptorCount))
        {
            bSuccess = true;
            break;
        }
    }

    if (!bSuccess)
    {
        bSuccess = AllocateNewHeap()->AllocateFreeDescriptorHeapBlock(NewHeapBlock, InDescriptorCount);
    }

    EA_ASSERT(bSuccess);
    return NewHeapBlock;
}

FD3D12DescriptorHeap* FD3D12DescriptorHeapContainer::AllocateNewHeap()
{
    FD3D12DescriptorHeap* Heap = nullptr;

    if (FreeDescriptorHeapList.size() > 0)
    {
        Heap = DescriptorHeapListAllocatedToUser.emplace_back(eastl::move(FreeDescriptorHeapList.front())).get();
        FreeDescriptorHeapList.pop();
    }
    else
    {
        Heap = DescriptorHeapListAllocatedToUser.emplace_back(eastl::make_unique<FD3D12DescriptorHeap>(NumDescriptor, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, HeapType)).get();
        Heap->Init();
    }

    return Heap;
}

void FD3D12DescriptorHeapContainer::FreeNewHeap(FD3D12DescriptorHeap* const InHeap)
{
    InHeap->MakeFreed();
    FreeDescriptorHeapList.emplace_back(InHeap);
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

void FD3D12DescriptorHeapManager::OnStartFrame()
{

}

void FD3D12DescriptorHeapManager::OnEndFrame()
{

}

FD3D12DescriptorHeapContainer* FD3D12DescriptorHeapManager::GetOnlineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
{
    switch (InHeapType)
    {
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        {
            return &RTVDescriptorHeapContainer;
        }
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        {
            return &DSVDescriptorHeapContainer;
        }
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        {
            return &CbvSrvUavOnlineDescriptorHeapContainer;
        }
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        {
            return &CbvSrvUavOfflineDescriptorHeapContainer;
        }
        default:
        {
            EA_ASSERT(false);
            return nullptr;
        }
    }
}

FD3D12DescriptorHeapContainer* FD3D12DescriptorHeapManager::GetOfflineDescriptorHeapContainer(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
{
    switch (InHeapType)
    {
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        {
            return &CbvSrvUavOnlineDescriptorHeapContainer;
        }
        case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        {
            return &SamplerOfflineDescriptorHeapContainer;
        }
        default:
        {
            EA_ASSERT(false);
            return nullptr;
        }
    }
}

FD3D12DescriptorHeapBlock FD3D12DescriptorHeapManager::AllocateOnlineHeapDescriptorHeapBlock(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const uint32 InDescriptorCount)
{
    return GetOnlineDescriptorHeapContainer(InHeapType)->AllocateDescriptorHeapBlock(InDescriptorCount);
}

FD3D12DescriptorHeapBlock FD3D12DescriptorHeapManager::AllocateOfflineHeapDescriptorHeapBlock(const D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, const uint32 InDescriptorCount)
{
    return GetOfflineDescriptorHeapContainer(InHeapType)->AllocateDescriptorHeapBlock(InDescriptorCount);
}
