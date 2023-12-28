#include "D3D12Device.h"

#include "D3D12Adapter.h"

FD3D12Adapter* FD3D12Device::GetAdapter()
{
    return Adapter;
}

ID3D12Device* FD3D12Device::GetD3D12Device()
{
    return Device.Get();
}

FD3D12CommandQueue& FD3D12Device::GetCommandQueue(const ED3D12QueueType QueueType)
{
    return CommandQueue[static_cast<uint64_t>(QueueType)];
}

FD3D12Device::FD3D12Device(FD3D12Adapter* const InAdapter, const bool bEnableDebugLayer)
    : Adapter(InAdapter)
{
    VERIFYD3D12RESULT(D3D12CreateDevice(Adapter->GetD3D12Adapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));

    for (uint64_t QueueTypeIndex = 0; QueueTypeIndex < static_cast<uint64_t>(ED3D12QueueType::Count); QueueTypeIndex)
    {
        CommandQueue.emplace_back(this, static_cast<ED3D12QueueType>(QueueTypeIndex));
    }
}

FD3D12Device* GetD3D12Device()
{
    return EA::StdC::Singleton<FD3D12Device>::GetInstance();
}

FD3D12Adapter* GetD3D12ChosenAdapter()
{
    return GetD3D12Device()->GetAdapter();
}
