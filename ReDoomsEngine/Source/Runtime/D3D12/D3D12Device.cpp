#include "D3D12Device.h"

#include "D3D12Adapter.h"

ID3D12Device* GetD3D12Device()
{
    if (FD3D12Device* const D3D12Device = FD3D12Device::GetInstance())
    {
        return D3D12Device->GetD3D12Device();
    }
    else
    {
        return nullptr;
    }
}

FD3D12Device::FD3D12Device(FD3D12Adapter* const InAdapter)
    : D3DDevice(), Adapter(InAdapter), CommandQueueList()
{
}

void FD3D12Device::Init()
{
    VERIFYD3D12RESULT(D3D12CreateDevice(Adapter->GetD3DAdapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));

    for (uint32_t QueueTypeIndex = 0; QueueTypeIndex < static_cast<uint32_t>(ED3D12QueueType::NumD3D12QueueType); ++QueueTypeIndex)
    {
        CommandQueueList[QueueTypeIndex] = eastl::make_unique<FD3D12CommandQueue>(static_cast<ED3D12QueueType>(QueueTypeIndex));
        CommandQueueList[QueueTypeIndex]->Init();
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS D3D12Caps;
    MEM_ZERO(D3D12Caps);

    VERIFYD3D12RESULT(D3DDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &D3D12Caps, sizeof(D3D12Caps)));
    ResourceHeapTier = D3D12Caps.ResourceHeapTier;
    ResourceBindingTier = D3D12Caps.ResourceBindingTier;

    D3D12_FEATURE_DATA_ROOT_SIGNATURE D3D12RootSignatureCaps = {};
    D3D12RootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;	// This is the highest version we currently support. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    if (FAILED(D3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &D3D12RootSignatureCaps, sizeof(D3D12RootSignatureCaps))))
    {
        D3D12RootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    RootSignatureVersion = D3D12RootSignatureCaps.HighestVersion;

    #if RD_DEBUG
    ComPtr<ID3D12InfoQueue> InfoQueue{};
    GetD3D12Device()->QueryInterface(IID_PPV_ARGS(&InfoQueue));
    if (InfoQueue)
	{
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
    #endif
}
