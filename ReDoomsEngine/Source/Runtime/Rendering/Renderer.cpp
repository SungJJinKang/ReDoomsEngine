#include "Renderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"

void FFrameResourceContainer::Init(eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> InOnlineDescriptorHeap)
{
	EA_ASSERT(!bInit);
	bInit = true;

	FrameWorkEndFence.Init();

	for (uint32_t CommandAllocatorIndex = 0; CommandAllocatorIndex < CommandAllocatorList.size(); ++CommandAllocatorIndex)
	{
		CommandAllocatorList[CommandAllocatorIndex] = eastl::make_shared<FD3D12CommandAllocator>(ED3D12QueueType::Direct);
		CommandAllocatorList[CommandAllocatorIndex]->Init();
	}
}

void FFrameResourceContainer::ClearFrameResource()
{
	EA_ASSERT(bInit);
}

void FFrameResourceContainer::OnPreStartFrame(FD3D12CommandContext& InCommandContext)
{
	EA_ASSERT(bInit);

	if (GCurrentFrameIndex >= GNumBackBufferCount)
	{
		FrameWorkEndFence.WaitOnLastSignal();
		ClearFrameResource();
	}
}

void FFrameResourceContainer::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
	EA_ASSERT(bInit);
}

void FFrameResourceContainer::OnEndFrame(FD3D12CommandContext& InCommandContext)
{
	EA_ASSERT(bInit);
}

void FRenderer::Init()
{
	SCOPED_MEMORY_TRACE(Renderer_Init)

	CurrentRendererState = ERendererState::Initializing;

	D3D12Manager.Init(this);

	eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> OnlineDescriptorHeap = eastl::make_shared<FD3D12OnlineDescriptorHeapContainer>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	OnlineDescriptorHeap->Init();
	for (FFrameResourceContainer& FrameContainer : FrameContainerList)
	{
		FrameContainer.Init(OnlineDescriptorHeap);
	}

	CurrentRendererState = ERendererState::FinishInitialzing;
}

void FRenderer::OnPreStartFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnPreStartFrame)

	CurrentRendererState = ERendererState::OnPreStartFrame;

	++GCurrentFrameIndex;

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameContainer();
	CurrentFrameCommandContext.StateCache.ResetForNewCommandlist();
	CurrentFrameCommandContext.FrameResourceCounter = &CurrentFrameContainer;
	CurrentFrameCommandContext.GraphicsCommandAllocator = CurrentFrameContainer.CommandAllocatorList[static_cast<uint32_t>(ECommandAllocatotrType::Graphics)];
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameCommandContext.GraphicsCommandAllocator->GetOrCreateNewCommandList();

	CurrentFrameContainer.OnPreStartFrame(CurrentFrameCommandContext);

	D3D12Manager.OnPreStartFrame(CurrentFrameCommandContext);
}

void FRenderer::OnStartFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)

	CurrentRendererState = ERendererState::OnStartFrame;

	GetCurrentFrameContainer().OnStartFrame(CurrentFrameCommandContext);

	D3D12Manager.OnStartFrame(CurrentFrameCommandContext);
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

	GetCurrentFrameContainer().OnPostEndFrame(CurrentFrameCommandContext);

	D3D12Manager.OnPostEndFrame(CurrentFrameCommandContext);
}

void FRenderer::OnEndFrame()
{
	SCOPED_MEMORY_TRACE(Renderer_OnEndFrame)

	CurrentRendererState = ERendererState::OnEndFrame;

	GetCurrentFrameContainer().OnEndFrame(CurrentFrameCommandContext);

	D3D12Manager.OnEndFrame(CurrentFrameCommandContext);

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