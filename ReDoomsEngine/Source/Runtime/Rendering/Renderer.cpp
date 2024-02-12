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

	++GCurrentFrameIndex;

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameContainer();
	CurrentFrameContainer.FrameWorkEndFence.WaitOnLastSignal();

	CurrentFrameContainer.ResetForNewFrame();
	CurrentFrameCommandContext.StateCache.ResetForNewCommandlist();
	CurrentFrameCommandContext.FrameResourceCounter = &CurrentFrameContainer;
	CurrentFrameCommandContext.GraphicsCommandAllocator = CurrentFrameContainer.CommandAllocatorList[static_cast<uint32_t>(ECommandAllocatotrType::Graphics)];
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameCommandContext.GraphicsCommandAllocator->GetOrCreateNewCommandList();

	D3D12Manager.OnPreStartFrame(CurrentFrameCommandContext);
}

void FRenderer::OnStartFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnStartFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)

	CurrentRendererState = ERendererState::OnStartFrame;

	D3D12Manager.OnStartFrame(CurrentFrameCommandContext);
	FImguiHelperSingleton::GetInstance()->NewFrame();
}

bool FRenderer::Draw()
{
	SCOPED_CPU_TIMER(Renderer_Draw)
	SCOPED_MEMORY_TRACE(Renderer_Draw)

	CurrentRendererState = ERendererState::Draw;

	eastl::shared_ptr<FD3D12Fence> ResourceUploadFence = FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.Flush();

	return true;
}

void FRenderer::OnPreEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnPreEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnPreEndFrame)

	CurrentRendererState = ERendererState::OnPreEndFrame;

	D3D12Manager.OnPreEndFrame(CurrentFrameCommandContext);
}

void FRenderer::OnPostEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnPostEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnPostEndFrame)

	CurrentRendererState = ERendererState::OnPostEndFrame;

	D3D12Manager.OnPostEndFrame(CurrentFrameCommandContext);
}

void FRenderer::OnEndFrame()
{
	SCOPED_CPU_TIMER(Renderer_OnEndFrame)
	SCOPED_MEMORY_TRACE(Renderer_OnEndFrame)

	CurrentRendererState = ERendererState::OnEndFrame;

	D3D12Manager.OnEndFrame(CurrentFrameCommandContext);

	FImguiHelperSingleton::GetInstance()->EndDraw(CurrentFrameCommandContext);

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();

	// Indicate that the back buffer will be used as a render target.
	FD3D12RenderTargetResource& TargetRenderTarget = SwapChain->GetRenderTarget(SwapChain->GetCurrentBackbufferIndex());
	// Indicate that the back buffer will now be used to present.
	CD3DX12_RESOURCE_BARRIER ResourceBarrierB = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget.GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CurrentFrameCommandContext.GraphicsCommandList->GetD3DCommandList()->ResourceBarrier(1, &ResourceBarrierB);

	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	eastl::vector<eastl::shared_ptr<FD3D12CommandList>> CommandLists = { CurrentFrameCommandContext.GraphicsCommandList };
	TargetCommandQueue->ExecuteCommandLists(CommandLists);

	SwapChain->Present(1);
	SwapChain->UpdateCurrentBackbufferIndex();

	GetCurrentFrameContainer().FrameWorkEndFence.Signal(TargetCommandQueue, false);
}

void FRenderer::Destroy()
{
	SCOPED_CPU_TIMER(Renderer_Destroy)
	SCOPED_MEMORY_TRACE(Renderer_Destroy)

	CurrentRendererState = ERendererState::Destroying;

	FImguiHelperSingleton::GetInstance()->OnDestory();

	D3D12Manager.OnDestory(CurrentFrameCommandContext);

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