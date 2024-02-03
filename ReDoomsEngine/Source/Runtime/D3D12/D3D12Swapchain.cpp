#include "D3D12Swapchain.h"
#include "D3D12Manager.h"
#include "D3D12CommandQueue.h"
#include "D3D12Window.h"
#include "D3D12Resource/D3D12Resource.h"

FD3D12Swapchain::FD3D12Swapchain(FD3D12CommandQueue* const InCommandQueue, FD3D12Window* const InWindow, const uint32_t InNumBuffer, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat)
    :
    D3DSwapchain(),
    CommandQueue(InCommandQueue),
    Window(InWindow),
    NumBuffer(InNumBuffer),
    Width(InWidth),
    Height(InHeight),
    Format(InFormat),
    CurrentBackbufferIndex(0)
{
}

void FD3D12Swapchain::Init()
{
    {
        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 Desc;
        MEM_ZERO(Desc);
        Desc.BufferCount = NumBuffer;
        Desc.Width = Width;
        Desc.Height = Height;
        Desc.Format = Format;
        Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
        Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        Desc.SampleDesc.Count = 1;
        Desc.SampleDesc.Quality = 0;

        ComPtr<IDXGISwapChain1> TempD3DSwapchain;

        VERIFYD3D12RESULT(FD3D12Manager::GetInstance()->GetDXGIFactory()->CreateSwapChainForHwnd(
            CommandQueue->GetD3DCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
            Window->GetWindowHandle(),
            &Desc,
            nullptr,
            nullptr,
            &TempD3DSwapchain
        ));

        VERIFYD3D12RESULT(TempD3DSwapchain.As(&D3DSwapchain));

        for (uint32_t BufferIndex = 0; BufferIndex < NumBuffer; ++BufferIndex)
        {
            ComPtr<ID3D12Resource> SwapChainBuffer{};

            VERIFYD3D12RESULT(D3DSwapchain->GetBuffer(BufferIndex, IID_PPV_ARGS(&SwapChainBuffer)));
            RenderTargets.emplace_back(SwapChainBuffer);
        }
    }
    
    UpdateCurrentBackbufferIndex();

    FD3D12Manager::GetInstance()->GetDXGIFactory()->MakeWindowAssociation(Window->GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
}

void FD3D12Swapchain::OnStartFrame()
{

}

void FD3D12Swapchain::OnEndFrame()
{

}

FD3D12RenderTargetResource& FD3D12Swapchain::GetD3DRenderTargetResource(const uint32_t InBufferIndex)
{
    return RenderTargets[InBufferIndex];
}

void FD3D12Swapchain::Present(const int32_t SyncInterval)
{
    GetD3DSwapchain()->Present(SyncInterval, 0);
}

void FD3D12Swapchain::UpdateCurrentBackbufferIndex()
{
    CurrentBackbufferIndex = GetD3DSwapchain()->GetCurrentBackBufferIndex();
}
