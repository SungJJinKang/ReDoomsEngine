#include "D3D12Manager.h"

#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Window.h"
#include "D3D12Swapchain.h"

static const uint32_t GNumBackBufferCount = 3;
static const int32_t GWindowWidth = 1024;
static const int32_t GWindowHeight = 720;


FD3D12Manager::FD3D12Manager(const bool bInEnableDebugLayer)
    : DXGIFactory(), D3D12Window(), ChoosenAdapter(), Swapchain(), bEnableDebugLayer(bInEnableDebugLayer)
{
}

FD3D12Manager::~FD3D12Manager()
{
    if (D3D12Window)
    {
        delete D3D12Window;
    }

    if (ChoosenAdapter)
    {
        delete ChoosenAdapter;
    }
}

void FD3D12Manager::Init()
{
    {
        D3D12Window = new FD3D12Window(GWindowWidth, GWindowHeight, EA_WCHAR("ReDoomsEngine"));
        D3D12Window->Init();
    }

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
    }

    {
        ChoosenAdapter = new FD3D12Adapter(true);
        ChoosenAdapter->Init();
    }

    {
        Swapchain = new FD3D12Swapchain(GetChoosenAdapter()->GetDevice()->GetCommandQueue(ED3D12QueueType::Direct),
            D3D12Window, GNumBackBufferCount, GWindowWidth, GWindowHeight, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
        Swapchain->Init();
    }
}