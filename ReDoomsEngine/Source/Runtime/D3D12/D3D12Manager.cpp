#include "D3D12Manager.h"

#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Window.h"
#include "D3D12Swapchain.h"
#include "D3D12Descriptor.h"
#include "D3D12Shader.h"
#include "D3D12PSO.h"
#include "D3D12RootSignature.h"
#include "D3D12CommandList.h"
#include "D3D12Resource/D3D12ConstantBufferRingBufferManager.h"


FD3D12Manager::FD3D12Manager() = default;
FD3D12Manager::~FD3D12Manager() = default;

void FD3D12Manager::Init()
{
    {
        D3D12Window = eastl::make_unique<FD3D12Window>(GWindowWidth, GWindowHeight, EA_WCHAR("ReDoomsEngine"));
        D3D12Window->Init();
        TickedManagerList.emplace_back(D3D12Window.get());
    }

    {
        uint32_t DXGIFactoryFlags = 0;

#if RD_DEBUG
        // Enable the debug layer (requires the Graphics Tools "optional feature").
        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        if (_DEBUG)
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
        TickedManagerList.emplace_back(ChoosenAdapter.get());
    }

    {
        D3D12CommandListManager = eastl::make_unique<FD3D12CommandListManager>();
        D3D12CommandListManager->Init();
        TickedManagerList.emplace_back(D3D12CommandListManager.get());
    }
    
    {
        Swapchain = eastl::make_unique<FD3D12Swapchain> (GetChoosenAdapter()->GetDevice()->GetCommandQueue(ED3D12QueueType::Direct),
            D3D12Window.get(), GNumBackBufferCount, GWindowWidth, GWindowHeight, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
        Swapchain->Init();
        TickedManagerList.emplace_back(Swapchain.get());
    }

    {
        D3D12DescriptorHeapManager = eastl::make_unique<FD3D12DescriptorHeapManager>();
        D3D12DescriptorHeapManager->Init();
        TickedManagerList.emplace_back(D3D12DescriptorHeapManager.get());
    }

    {
        D3D12RootSignatureManager = eastl::make_unique<FD3D12RootSignatureManager>();
        D3D12RootSignatureManager->Init();
        TickedManagerList.emplace_back(D3D12RootSignatureManager.get());
    }

    {
        D3D12ShaderManager = eastl::make_unique<FD3D12ShaderManager>();
        D3D12ShaderManager->Init();
        TickedManagerList.emplace_back(D3D12ShaderManager.get());
    }

    {
        D3D12PSOManager = eastl::make_unique<FD3D12PSOManager>();
        TickedManagerList.emplace_back(D3D12PSOManager.get());
    }

    {
        D3D12ConstantBufferRingBufferManager = eastl::make_unique<FD3D12ConstantBufferRingBufferManager>();
        D3D12ConstantBufferRingBufferManager->Init();
        TickedManagerList.emplace_back(D3D12ConstantBufferRingBufferManager.get());
    }
}

void FD3D12Manager::OnPreStartFrame()
{
    for (ID3D12ManagerInterface* Manager : TickedManagerList)
    {
        Manager->OnPreStartFrame();
    }
}

void FD3D12Manager::OnStartFrame()
{
    for (ID3D12ManagerInterface* Manager : TickedManagerList)
    {
        Manager->OnStartFrame();
    }
}

void FD3D12Manager::OnEndFrame()
{
    for (ID3D12ManagerInterface* Manager : TickedManagerList)
    {
        Manager->OnEndFrame();
    }
}

void FD3D12Manager::OnPostEndFrame()
{
    for (ID3D12ManagerInterface* Manager : TickedManagerList)
    {
        Manager->OnPostEndFrame();
    }
}
