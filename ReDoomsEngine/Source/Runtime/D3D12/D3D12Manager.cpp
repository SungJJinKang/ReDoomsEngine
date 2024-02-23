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
#include "D3D12Resource/D3D12ConstantBufferRingBuffer.h"
#include "D3D12Resource/D3D12ResourceAllocator.h"

FD3D12Manager::FD3D12Manager() = default;
FD3D12Manager::~FD3D12Manager() = default;

void FD3D12Manager::Init(FRenderer* const InRenderer)
{
    {
        D3D12Window = eastl::make_unique<FD3D12Window>(GWindowWidth, GWindowHeight, EA_WCHAR("ReDoomsEngine"), InRenderer);
        D3D12Window->Init();
        TickedManagerList.emplace_back(D3D12Window.get());
    }

    {
        uint32_t DXGIFactoryFlags = 0;

#if RD_DEBUG
        // Enable the debug layer (requires the Graphics Tools "optional feature").
        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
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
        Swapchain = eastl::make_unique<FD3D12Swapchain> (&(GetChoosenAdapter()->GetDevice()->GetCommandQueue(ED3D12QueueType::Direct)),
            D3D12Window.get(), GNumBackBufferCount, GWindowWidth, GWindowHeight, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 0);
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
        D3D12ConstantBufferRingBuffer = eastl::make_unique<FD3D12ConstantBufferRingBuffer>();
        D3D12ConstantBufferRingBuffer->Init();
        TickedManagerList.emplace_back(D3D12ConstantBufferRingBuffer.get());
    }

    {
        D3D12ResourceAllocator = eastl::make_unique<FD3D12ResourceAllocator>();
        D3D12ResourceAllocator->Init();
        TickedManagerList.emplace_back(D3D12ResourceAllocator.get());
    }
}

void FD3D12Manager::OnPreStartFrame()
{
    for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
    {
        Manager->OnPreStartFrame();
    }
}

void FD3D12Manager::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
    for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
    {
        Manager->OnStartFrame(InCommandContext);
    }
}

void FD3D12Manager::OnPreEndFrame(FD3D12CommandContext& InCommandContext)
{
	for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
	{
		Manager->OnPreEndFrame(InCommandContext);
	}
}

void FD3D12Manager::OnEndFrame(FD3D12CommandContext& InCommandContext)
{
    for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
    {
        Manager->OnEndFrame(InCommandContext);
    }
}

void FD3D12Manager::OnPostEndFrame()
{
    for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
    {
        Manager->OnPostEndFrame();
    }
}

void FD3D12Manager::OnDestory(FD3D12CommandContext& InCommandContext)
{
	for (ID3D12RendererStateCallbackInterface* Manager : TickedManagerList)
	{
		Manager->OnDestory(InCommandContext);
	}
}
