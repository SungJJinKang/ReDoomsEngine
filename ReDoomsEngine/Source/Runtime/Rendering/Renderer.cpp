#include "Renderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"

void FFrameResourceContainer::Init()
{
	FrameWorkEndFence.Init();

	for (uint32_t CommandAllocatorIndex = 0; CommandAllocatorIndex < CommandAllocatorList.size(); ++CommandAllocatorIndex)
	{
		CommandAllocatorList[CommandAllocatorIndex] = eastl::make_shared<FD3D12CommandAllocator>(ED3D12QueueType::Direct);
		CommandAllocatorList[CommandAllocatorIndex]->Init();
	}

	SrvUavOnlineDescriptorHeapContainer->Init();
}

void FFrameResourceContainer::ClearFrameResource()
{
	SrvUavOnlineDescriptorHeapContainer->Reset();
}

void FFrameResourceContainer::OnStartFrame()
{
	if (GCurrentFrameIndex >= GNumBackBufferCount)
	{
		FrameWorkEndFence.WaitOnLastSignal();
		ClearFrameResource();
	}
}

void FFrameResourceContainer::OnEndFrame()
{

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

	++GCurrentFrameIndex;

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameContainer();
	CurrentFrameContainer.OnStartFrame();

	CurrentFrameCommandContext.StateCache.Reset();
	CurrentFrameCommandContext.FrameResourceCounter = &CurrentFrameContainer;
	CurrentFrameCommandContext.GraphicsCommandAllocator = CurrentFrameContainer.CommandAllocatorList[static_cast<uint32_t>(ECommandAllocatotrType::Graphics)];
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameCommandContext.GraphicsCommandAllocator->GetOrCreateNewCommandList();

	// @todo replace per frame online heap with heap independent from frame?
	// Changing set descriptor heap flushes gpu pipeline.
	CurrentFrameCommandContext.StateCache.SetDescriptorHeaps(CurrentFrameCommandContext, { CurrentFrameContainer.SrvUavOnlineDescriptorHeapContainer->GetOnlineHeap() });

	D3D12Manager.OnPreStartFrame();
}

void FRenderer::OnStartFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)

	CurrentRendererState = ERendererState::OnStartFrame;

	D3D12Manager.OnStartFrame();
}

bool FRenderer::Draw()
{
	SCOPED_MEMORY_TRACE(Renderer_Draw)

	CurrentRendererState = ERendererState::Draw;

	eastl::shared_ptr<FD3D12Fence> ResourceUploadFence = FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.Flush();

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
