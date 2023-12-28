#include "D3D12Manager.h"

#include "D3D12Adapter.h"

FD3D12Manager::FD3D12Manager()
    : DXGIFactory(), ChoosenAdapter(nullptr)
{
}

void FD3D12Manager::Init(const bool bEnableDebugLayer)
{
    uint32_t DXGIFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    if (bEnableDebugLayer)
    {
        ComPtr<ID3D12Debug> DebugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
        {
            DebugController->EnableDebugLayer();

            // Enable additional debug layers.
            DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    VERIFYD3D12RESULT(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&DXGIFactory)));


    {
        ChoosenAdapter = new FD3D12Adapter(*(DXGIFactory.Get()), true);
        ChoosenAdapter->CreateDevice();
        
    }
}

IDXGIFactory4* FD3D12Manager::GetDXGIFactory()
{
	return DXGIFactory.Get();
}
