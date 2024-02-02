#include "Renderer.h"

void FFrameResourceContainer::Init()
{
	FrameWorkEndFence.Init();

	GraphicsCommandAllocator = FD3D12CommandListManager::GetInstance()->GetOrCreateNewCommandAllocator(ED3D12QueueType::Direct);
	GraphicsCommandList = GraphicsCommandAllocator->GetOrCreateNewCommandList();
}

void FRenderer::Init()
{
	D3D12Manager.Init();

	for (FFrameResourceContainer& FrameContainer : FrameContainerList)
	{
		FrameContainer.Init();
	}
}

void FRenderer::OnPreStartFrame()
{
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
	D3D12Manager.OnStartFrame();

	// @todo : Block if gpu work of "GCurrentFrameIndex - GNumBackBufferCount" Frame doesn't finish yet

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameContainer();
	CurrentFrameContainer.GraphicsCommandAllocator->ResetCommandAllocator(false);

	CurrentFrameCommandContext.CommandList = CurrentFrameContainer.GraphicsCommandList;
}

bool FRenderer::Draw()
{
	return true;
}

void FRenderer::OnPostEndFrame()
{
	D3D12Manager.OnPostEndFrame();
}

void FRenderer::OnEndFrame()
{
	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	CurrentFrameCommandContext.CommandList->FinishRecordingCommandList(TargetCommandQueue);

	ID3D12CommandList* CommandLists[] = { CurrentFrameCommandContext.CommandList->GetD3DCommandList() };
	TargetCommandQueue->GetD3DCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	GetCurrentFrameContainer().FrameWorkEndFence.Signal(TargetCommandQueue, false);

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	SwapChain->Present(1);
	SwapChain->UpdateCurrentBackbufferIndex();

	D3D12Manager.OnEndFrame();
}

void FRenderer::Destroy()
{
	GetCurrentFrameContainer().FrameWorkEndFence.WaitOnLastSignal();
}

FFrameResourceContainer& FRenderer::GetCurrentFrameContainer()
{
	return FrameContainerList[GCurrentFrameIndex % GNumBackBufferCount];
}
