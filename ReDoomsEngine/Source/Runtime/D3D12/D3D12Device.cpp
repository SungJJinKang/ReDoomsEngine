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
        FD3D12CommandQueue& CommandQueue = CommandQueueList.emplace_back(static_cast<ED3D12QueueType>(QueueTypeIndex));
        CommandQueue.Init();
    }
}
