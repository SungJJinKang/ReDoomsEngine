#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Manager.h"

FD3D12Adapter::FD3D12Adapter(const bool bInRequestHighPerformanceAdapter)
    :
    D3DAdapter(),
    Device(),
    bRequestHighPerformanceAdapter(bInRequestHighPerformanceAdapter)
{
}

FD3D12Adapter::~FD3D12Adapter()
{
    if (Device)
    {
        delete Device;
    }
}

void FD3D12Adapter::Init()
{
    {
        ComPtr<IDXGIAdapter1> LocalAdapter;

        ComPtr<IDXGIFactory6> Factory6;
        if (SUCCEEDED(FD3D12Manager::GetInstance()->GetDXGIFactory()->QueryInterface(IID_PPV_ARGS(&Factory6))))
        {
            for (uint32_t AdapterIndex = 0;
                SUCCEEDED(Factory6->EnumAdapterByGpuPreference(
                    AdapterIndex,
                    bRequestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&LocalAdapter)));
                ++AdapterIndex)
            {
                DXGI_ADAPTER_DESC1 AdapterDesc;
                MEM_ZERO(AdapterDesc);
                LocalAdapter->GetDesc1(&AdapterDesc);

                if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(LocalAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        if (LocalAdapter.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(FD3D12Manager::GetInstance()->GetDXGIFactory()->EnumAdapters1(adapterIndex, &LocalAdapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 AdapterDesc;
                MEM_ZERO(AdapterDesc);
                LocalAdapter->GetDesc1(&AdapterDesc);

                if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(LocalAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        D3DAdapter = LocalAdapter;
    }

    {
        EA_ASSERT(!Device);

        Device = new FD3D12Device(this);
        Device->Init();
    }
}

void FD3D12Adapter::OnStartFrame()
{

}

void FD3D12Adapter::OnEndFrame()
{

}
