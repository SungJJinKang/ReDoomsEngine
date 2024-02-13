#include "Renderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "Editor/ImguiHelper.h"

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

void FFrameResourceContainer::ResetForNewFrame()
{
	EA_ASSERT(bInit);

	for (eastl::shared_ptr<FD3D12CommandAllocator>& CommandAllocator : CommandAllocatorList)
	{
		CommandAllocator->ResetCommandAllocator();
	}
}

void FRenderer::Init()
{
	SCOPED_CPU_TIMER(Renderer_Init)
	SCOPED_MEMORY_TRACE(Renderer_Init)

	CurrentRendererState = ERendererState::Initializing;

	D3D12Manager.Init(this);

	eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> OnlineDescriptorHeap = eastl::make_shared<FD3D12OnlineDescriptorHeapContainer>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	OnlineDescriptorHeap->Init();
	for (FFrameResourceContainer& FrameContainer : FrameContainerList)
	{
		FrameContainer.Init(OnlineDescriptorHeap);
	}

	FImguiHelperSingleton::GetInstance()->Init();

	CurrentRendererState = ERendererState::FinishInitialzing;
}

void FRenderer::OnPreStartFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnPreStartFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnPreStartFrame)

	CurrentRendererState = ERendererState::OnPreStartFrame;

	D3D12Manager.OnPreStartFrame();
}

void FRenderer::OnStartFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnStartFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)

	CurrentRendererState = ERendererState::OnStartFrame;

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameResourceContainer();
	CurrentFrameContainer.FrameWorkEndFence.WaitOnLastSignal();

	CurrentFrameContainer.ResetForNewFrame();
	CurrentFrameCommandContext.StateCache.ResetForNewCommandlist();
	CurrentFrameCommandContext.FrameResourceCounter = &CurrentFrameContainer;
	CurrentFrameCommandContext.CommandAllocatorList = CurrentFrameContainer.CommandAllocatorList;
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameContainer.CommandAllocatorList[static_cast<uint32_t>(ECommandAllocatotrType::Graphics)]->GetOrCreateNewCommandList();

	D3D12Manager.OnStartFrame(CurrentFrameCommandContext);
	FImguiHelperSingleton::GetInstance()->NewFrame();
}

bool FRenderer::Draw()
{
	SCOPED_CPU_TIMER(Renderer_Draw)
	SCOPED_MEMORY_TRACE(Renderer_Draw)

	CurrentRendererState = ERendererState::Draw;

	eastl::shared_ptr<FD3D12Fence> ResourceUploadFence = FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.Flush(CurrentFrameCommandContext);

	return true;
}

void FRenderer::OnPreEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnPreEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnPreEndFrame)

	CurrentRendererState = ERendererState::OnPreEndFrame;

	D3D12Manager.OnPreEndFrame(CurrentFrameCommandContext);

	FImguiHelperSingleton::GetInstance()->EndDraw(CurrentFrameCommandContext);

	CD3DX12_CPU_DESCRIPTOR_HANDLE NullRTV = FD3D12RenderTargetView::NullRTV()->GetDescriptorHeapBlock().CPUDescriptorHandle();
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->OMSetRenderTargets(1, &NullRTV, FALSE, nullptr);
}

void FRenderer::OnPostEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnPostEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnPostEndFrame)

	CurrentRendererState = ERendererState::OnPostEndFrame;

	D3D12Manager.OnPostEndFrame();
}

void FRenderer::OnEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnEndFrame)

	CurrentRendererState = ERendererState::OnEndFrame;

	D3D12Manager.OnEndFrame(CurrentFrameCommandContext);

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	// Indicate that the back buffer will be used as a render target.
	FD3D12RenderTargetResource& TargetRenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);

	// Indicate that the back buffer will now be used to present.
	CD3DX12_RESOURCE_BARRIER ResourceBarrierB = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierB);

	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	eastl::vector<eastl::shared_ptr<FD3D12CommandList>> CommandLists = { CurrentFrameCommandContext.GraphicsCommandList };
	TargetCommandQueue->ExecuteCommandLists(CommandLists);

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameResourceContainer();
	SwapChain->Present(0);
	SwapChain->UpdateCurrentBackbufferIndex();

	CurrentFrameContainer.FrameWorkEndFence.Signal(TargetCommandQueue, false);
}

void FRenderer::Destroy()
{
	SCOPED_CPU_TIMER(Renderer_Destroy)
	SCOPED_MEMORY_TRACE(Renderer_Destroy)

	CurrentRendererState = ERendererState::Destroying;

	FImguiHelperSingleton::GetInstance()->OnDestory();

	D3D12Manager.OnDestory(CurrentFrameCommandContext);

	GetCurrentFrameResourceContainer().FrameWorkEndFence.WaitOnLastSignal();
}

FFrameResourceContainer& FRenderer::GetPreviousFrameResourceContainer()
{
	EA_ASSERT(CurrentRendererState | ERendererState::FrameResourceAccessible);

	const uint64_t CurrentBackbufferIndex = GCurrentBackbufferIndex;
	return FrameContainerList[(CurrentBackbufferIndex == 0) ? (GNumBackBufferCount - 1) : (CurrentBackbufferIndex - 1)];
}

FFrameResourceContainer& FRenderer::GetCurrentFrameResourceContainer()
{
	EA_ASSERT(CurrentRendererState | ERendererState::FrameResourceAccessible);
	
	return FrameContainerList[GCurrentBackbufferIndex];
}

eastl::array<FFrameResourceContainer, GNumBackBufferCount>& FRenderer::GetFrameContainerList()
{
	return FrameContainerList;
}
