#include "D3D12Swapchain.h"
#include "D3D12Manager.h"
#include "D3D12CommandQueue.h"
#include "D3D12Window.h"

FD3D12Swapchain::FD3D12Swapchain(FD3D12CommandQueue* const InCommandQueue, FD3D12Window* const InWindow, const uint32_t InNumBuffer, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat)
    :
    D3DSwapchain(),
    CommandQueue(InCommandQueue),
    Window(InWindow),
    NumBuffer(InNumBuffer),
    Width(InWidth),
    Height(InHeight),
    Format(InFormat)
{
}

void FD3D12Swapchain::Init()
{// Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 Desc = {};
    Desc.BufferCount = NumBuffer;
    Desc.Width = Width;
    Desc.Height = Height;
    Desc.Format = Format;
    Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;

    VERIFYD3D12RESULT(FD3D12Manager::GetInstance()->GetDXGIFactory()->CreateSwapChainForHwnd(
        CommandQueue->GetD3DCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
        Window->GetWindowHandle(),
        &Desc,
        nullptr,
        nullptr,
        &D3DSwapchain
    ));


    FD3D12Manager::GetInstance()->GetDXGIFactory()->MakeWindowAssociation(Window->GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
}

void FD3D12Swapchain::Present(const int32_t SyncInterval)
{
    GetD3DSwapchain()->Present(SyncInterval, 0);
}
