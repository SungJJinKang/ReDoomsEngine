#include "Renderer.h"

void FFrameResourceContainer::Init()
{
	FrameWorkEndFence.Init();

	GraphicsCommandAllocator = FD3D12CommandListManager::GetInstance()->GetOrCreateNewCommandAllocator(ED3D12QueueType::Direct);
	GraphicsCommandList = GraphicsCommandAllocator->GetOrCreateNewCommandList();
}

void FRenderer::Init()
{
	SCOPED_MEMORY_TRACE(Renderer_Init)

	CurrentRendererState = ERendererState::Initializing;

	D3D12Manager.Init(this);

	for (FFrameResourceContainer& FrameContainer : FrameContainerList)
	{
		FrameContainer.Init();
	}

	CurrentRendererState = ERendererState::FinishInitialzing;
}

void FRenderer::OnPreStartFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnPreStartFrame)

	CurrentRendererState = ERendererState::OnPreStartFrame;

	if (GCurrentFrameIndex >= GNumBackBufferCount)
	{
		FFrameResourceContainer& FrameContainer = FrameContainerList[(GCurrentFrameIndex - GNumBackBufferCount) % GNumBackBufferCount];
		FrameContainer.FrameWorkEndFence.WaitOnLastSignal();
	}

	++GCurrentFrameIndex;

	D3D12Manager.OnPreStartFrame();
}

void FRenderer::OnStartFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)

	CurrentRendererState = ERendererState::OnStartFrame;

	D3D12Manager.OnStartFrame();

	// @todo : Block if gpu work of "GCurrentFrameIndex - GNumBackBufferCount" Frame doesn't finish yet

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameContainer();

	CurrentFrameCommandContext.StateCache.Reset();
	CurrentFrameCommandContext.GraphicsCommandAllocator = CurrentFrameContainer.GraphicsCommandAllocator;
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameContainer.GraphicsCommandList;
}

bool FRenderer::Draw()
{
	SCOPED_MEMORY_TRACE(Renderer_Draw)

	CurrentRendererState = ERendererState::Draw;

	return true;
}

void FRenderer::OnPostEndFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnPostEndFrame)

	CurrentRendererState = ERendererState::OnPostEndFrame;

	D3D12Manager.OnPostEndFrame();
}

void FRenderer::OnEndFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnEndFrame)

	CurrentRendererState = ERendererState::OnEndFrame;

	D3D12Manager.OnEndFrame();

	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	eastl::vector<eastl::shared_ptr<FD3D12CommandList>> CommandLists = { CurrentFrameCommandContext.GraphicsCommandList };
	TargetCommandQueue->ExecuteCommandLists(CommandLists);

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	SwapChain->Present(1);
	SwapChain->UpdateCurrentBackbufferIndex();

	GetCurrentFrameContainer().FrameWorkEndFence.Signal(TargetCommandQueue, false);
}

void FRenderer::Destroy()
{
	SCOPED_MEMORY_TRACE(Renderer_Destroy)

	CurrentRendererState = ERendererState::Destroying;

	GetCurrentFrameContainer().FrameWorkEndFence.WaitOnLastSignal();
}

FFrameResourceContainer& FRenderer::GetLastFrameContainer()
{
	return FrameContainerList[(GCurrentFrameIndex - 1) % GNumBackBufferCount];
}

FFrameResourceContainer& FRenderer::GetCurrentFrameContainer()
{
	return FrameContainerList[GCurrentFrameIndex % GNumBackBufferCount];
}
