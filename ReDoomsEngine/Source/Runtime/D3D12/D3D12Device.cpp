#include "D3D12Device.h"

#include "D3D12Adapter.h"

FD3D12Device::FD3D12Device(FD3D12Adapter* const InAdapter)
    : Adapter(InAdapter)
{
}

void FD3D12Device::Init()
{
    VERIFYD3D12RESULT(D3D12CreateDevice(Adapter->GetD3DAdapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));

    for (uint64_t QueueTypeIndex = 0; QueueTypeIndex < static_cast<uint64_t>(ED3D12QueueType::Count); ++QueueTypeIndex)
    {
        FD3D12CommandQueue& CommandQueue = CommandQueueList.emplace_back(this, static_cast<ED3D12QueueType>(QueueTypeIndex));
        CommandQueue.Init();
    }
}

FD3D12Adapter* FD3D12Device::GetAdapter()
{
    return Adapter;
}

ID3D12Device* FD3D12Device::GetD3D12Device()
{
    return D3DDevice.Get();
}

FD3D12CommandQueue* FD3D12Device::GetCommandQueue(const ED3D12QueueType QueueType)
{
    return &CommandQueueList[static_cast<uint64_t>(QueueType)];
}

FD3D12Device* GetD3D12Device()
{
    return EA::StdC::Singleton<FD3D12Device>::GetInstance();
}

FD3D12Adapter* GetD3D12ChosenAdapter()
{
    return GetD3D12Device()->GetAdapter();
}
