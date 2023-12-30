#include "D3D12Descriptor.h"

#include "D3D12Device.h"

static uint32_t GetNumDescriptorsForOnlineHeap(const D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
    return 0;
}

static uint32_t GetNumDescriptorsForOfflineHeap(const D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
    return 0;
}

FD3D12DescriptorHeap::FD3D12DescriptorHeap(uint32_t InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InHeapFlags, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType)
	: NumDescriptors(InNumDescriptors), HeapFlags(InHeapFlags), HeapType(InHeapType), DescriptorSize(), CPUBase(), GPUBase(), FreeSlot()
{
}

void FD3D12DescriptorHeap::Init()
{
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
    DescriptorHeapDesc.NumDescriptors = NumDescriptors;
    DescriptorHeapDesc.Type = HeapType;
    DescriptorHeapDesc.Flags = HeapFlags;
    VERIFYD3D12RESULT(FD3D12Device::GetInstance()->GetD3D12Device()->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3DDescriptorHeap)));

    DescriptorSize = FD3D12Device::GetInstance()->GetD3D12Device()->GetDescriptorHandleIncrementSize(HeapType);

    CPUBase = D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    GPUBase = D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

    FreeSlot.set(NumDescriptors, true);
}

void FD3D12DescriptorHeapManager::Init()
{
    

   
}

void FD3D12OnlineDescriptorHeapManager::Init()
{
    DescriptorHeapPerType.resize(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    for (size_t HeapTypeIndex = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; HeapTypeIndex < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++HeapTypeIndex)
    {
        const D3D12_DESCRIPTOR_HEAP_TYPE HeapType = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(HeapTypeIndex);
        eastl::unique_ptr<FD3D12DescriptorHeap>& OnlineHeap =
            DescriptorHeapPerType[HeapTypeIndex].DescriptorHeapList.emplace_back(eastl::make_unique<FD3D12DescriptorHeap>(GetNumDescriptorsForOnlineHeap(HeapType), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, HeapType));
        OnlineHeap->Init();
    }
}

void FD3D12OfflineDescriptorHeapManager::Init()
{
    DescriptorHeapPerType.resize(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    for (size_t HeapTypeIndex = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; HeapTypeIndex < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++HeapTypeIndex)
    {
        const D3D12_DESCRIPTOR_HEAP_TYPE HeapType = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(HeapTypeIndex);
        eastl::unique_ptr<FD3D12DescriptorHeap>& OfflineHeap =
            DescriptorHeapPerType[HeapTypeIndex].DescriptorHeapList.emplace_back(eastl::make_unique<FD3D12DescriptorHeap>(GetNumDescriptorsForOfflineHeap(HeapType), D3D12_DESCRIPTOR_HEAP_FLAG_NONE, HeapType));
        OfflineHeap->Init();
    }
}
