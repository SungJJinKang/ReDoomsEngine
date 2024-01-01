#include "D3D12Manager.h"

#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Window.h"
#include "D3D12Swapchain.h"
#include "D3D12Descriptor.h"
#include "D3D12Shader.h"
#include "D3D12RootSignature.h"

static const uint32_t GNumBackBufferCount = 3;
static const int32_t GWindowWidth = 1024;
static const int32_t GWindowHeight = 720;


FD3D12Manager::FD3D12Manager(const bool bInEnableDebugLayer)
    :
    DXGIFactory(), 
    D3D12Window(),
    ChoosenAdapter(),
    Swapchain(),
    bEnableDebugLayer(bInEnableDebugLayer), 
    D3D12DescriptorHeapManager(),
    D3D12ShaderManager(),
    D3D12RootSignatureManager()
{
}

FD3D12Manager::~FD3D12Manager() = default;

void FD3D12Manager::Init()
{
    {
        D3D12Window = eastl::make_unique<FD3D12Window>(GWindowWidth, GWindowHeight, EA_WCHAR("ReDoomsEngine"));
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
        ChoosenAdapter = eastl::make_unique<FD3D12Adapter>(true);
        ChoosenAdapter->Init();
    }

    {
        Swapchain = eastl::make_unique<FD3D12Swapchain> (GetChoosenAdapter()->GetDevice()->GetCommandQueue(ED3D12QueueType::Direct),
            D3D12Window.get(), GNumBackBufferCount, GWindowWidth, GWindowHeight, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
        Swapchain->Init();
    }

    {
        D3D12DescriptorHeapManager = eastl::make_unique<FD3D12DescriptorHeapManager>();
        D3D12DescriptorHeapManager->Init();
    }

    {
        D3D12RootSignatureManager = eastl::make_unique<FD3D12RootSignatureManager>();
        D3D12RootSignatureManager->Init();
    }

    {
        D3D12ShaderManager = eastl::make_unique<FD3D12ShaderManager>();
        D3D12ShaderManager->Init();
    }
    
}