#include "D3D12Swapchain.h"
#include "D3D12Manager.h"
#include "D3D12CommandQueue.h"
#include "D3D12Window.h"
#include "D3D12Resource/D3D12Resource.h"
#include "Renderer.h"

FD3D12Swapchain::FD3D12Swapchain(FD3D12CommandQueue* const InCommandQueue, FD3D12Window* const InWindow, const uint32_t InNumBuffer, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat)
    :
    D3DSwapchain(),
    CommandQueue(InCommandQueue),
    Window(InWindow),
    NumBuffer(InNumBuffer),
    Width(InWidth),
    Height(InHeight),
    ResizedWidth(InWidth),
    ResizedHeight(InHeight),
    Format(InFormat)
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

        CreateRenderTargets();
    }
    
    UpdateCurrentBackbufferIndex();

    FD3D12Manager::GetInstance()->GetDXGIFactory()->MakeWindowAssociation(Window->GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
}

bool FD3D12Swapchain::IsRequireResize() const
{
    return (Width != ResizedWidth) || (Height != ResizedHeight);
}

void FD3D12Swapchain::CreateRenderTargets()
{
	for (uint32_t BufferIndex = 0; BufferIndex < NumBuffer; ++BufferIndex)
	{
		ComPtr<ID3D12Resource> SwapChainBuffer{};

		VERIFYD3D12RESULT(D3DSwapchain->GetBuffer(BufferIndex, IID_PPV_ARGS(&SwapChainBuffer)));
		RenderTargets.emplace_back(eastl::make_shared<FD3D12RenderTargetResource>(SwapChainBuffer))->InitResource();
	}
}

void FD3D12Swapchain::ResizeIfRequired()
{
	if (IsRequireResize())
	{
        FFrameResourceContainer& PreviousFrameContainer = FRenderer::GetInstance()->GetPreviousFrameResourceContainer();
        PreviousFrameContainer.FrameWorkEndFence.CPUWaitOnLastSignal();
       
        RenderTargets.clear();
		VERIFYD3D12RESULT(D3DSwapchain->ResizeBuffers(NumBuffer, (UINT)LOWORD(ResizedWidth), (UINT)HIWORD(ResizedHeight), Format, 0));
        UpdateCurrentBackbufferIndex();

		CreateRenderTargets();

		Width = ResizedWidth;
		Height = ResizedHeight;
	}
}

void FD3D12Swapchain::QueueResize(const uint32_t InWidth, const uint32_t InHeight)
{
    if ((Width != InWidth) || (Height != InHeight))
    {
        ResizedWidth = InWidth;
        ResizedHeight = InHeight;
    }
}

void FD3D12Swapchain::OnPreStartFrame()
{
    ResizeIfRequired();
}

void FD3D12Swapchain::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
}

void FD3D12Swapchain::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}

void FD3D12Swapchain::Present(const int32_t SyncInterval)
{
    VERIFYD3D12RESULT(GetD3DSwapchain()->Present(SyncInterval, 0));
}

void FD3D12Swapchain::UpdateCurrentBackbufferIndex()
{
    GCurrentBackbufferIndex = GetD3DSwapchain()->GetCurrentBackBufferIndex();
}
