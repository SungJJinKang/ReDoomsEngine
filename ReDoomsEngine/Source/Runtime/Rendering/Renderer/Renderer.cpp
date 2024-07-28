#include "Renderer/Renderer.h"

#include "D3D12Resource/D3D12ResourceAllocator.h"
#include "Editor/ImguiHelper.h"

void FFrameResourceContainer::Init(eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> InOnlineDescriptorHeap)
{
	EA_ASSERT(!bInit);
	bInit = true;

	FrameWorkEndFence = eastl::make_shared<FD3D12Fence>();
	FrameWorkEndFence->InitIfRequired();

	for (uint32_t CommandAllocatorIndex = 0; CommandAllocatorIndex < CommandAllocatorList.size(); ++CommandAllocatorIndex)
	{
		CommandAllocatorList[CommandAllocatorIndex] = eastl::make_shared<FD3D12CommandAllocator>(CommandAllocatorTypeTiD3D12QueueType(static_cast<ECommandAllocatorType>(CommandAllocatorIndex)));
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

	TransientFrameWorkEndFenceList.clear();

	for (eastl::weak_ptr<FD3D12Resource>& WeakPtrDeferredDeletedResource : DeferredDeletedResourceList)
	{
		if (eastl::shared_ptr<FD3D12Resource> SharedPtrDeferredDeletedResource = WeakPtrDeferredDeletedResource.lock())
		{
			EA_ASSERT(SharedPtrDeferredDeletedResource.unique());
			SharedPtrDeferredDeletedResource->ReleaseResource();
		}
	}
	DeferredDeletedResourceList.clear();
}

void FRenderer::Init()
{
	GCurrentRendererState = ERendererState::Initializing;

	JobSystem.Init();
	D3D12Manager.Init(this);

	eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> OnlineDescriptorHeap = eastl::make_shared<FD3D12OnlineDescriptorHeapContainer>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	OnlineDescriptorHeap->Init();
	for (FFrameResourceContainer& FrameContainer : FrameContainerList)
	{
		FrameContainer.Init(OnlineDescriptorHeap);
	}

	FImguiHelperSingleton::GetInstance()->Init();

	CurrentFrameCommandContext.StateCache.ResetToDefault();

	RenderScene.Init();
	RenderScene.RenderObjectList.Reserve(500);
}

void FRenderer::SceneSetup()
{
	GCurrentRendererState = ERendererState::SceneSetup;
}

void FRenderer::OnPreStartFrame()
{
	GCurrentRendererState = ERendererState::OnPreStartFrame;

	D3D12Manager.OnPreStartFrame();
}

void FRenderer::OnStartFrame()
{
	GCurrentRendererState = ERendererState::OnStartFrame;

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameResourceContainer();
	{
		SCOPED_CPU_TIMER(Renderer_OnStartFrame_WaitOnFrameWorkFence)
		CurrentFrameContainer.FrameWorkEndFence->CPUWaitOnLastSignal();
		for (eastl::shared_ptr<FD3D12Fence>& TransientFrameWorkEndFence : CurrentFrameContainer.TransientFrameWorkEndFenceList)
		{
			TransientFrameWorkEndFence->CPUWaitOnLastSignal();
		}
	}

	CurrentFrameContainer.ResetForNewFrame();
	CurrentFrameCommandContext.FrameResourceCounter = &CurrentFrameContainer;
	for (uint32_t QueueIndex = 0; QueueIndex < ED3D12QueueType::NumD3D12QueueType; ++QueueIndex)
	{
		CurrentFrameCommandContext.CommandQueueList[QueueIndex] = &(FD3D12Device::GetInstance()->GetCommandQueue(static_cast<ED3D12QueueType>(QueueIndex)));
	}
	CurrentFrameCommandContext.CommandAllocatorList = CurrentFrameContainer.CommandAllocatorList;
	CurrentFrameCommandContext.GraphicsCommandList = CurrentFrameContainer.CommandAllocatorList[static_cast<uint32_t>(ECommandAllocatorType::Graphics)]->GetOrCreateNewCommandList();
	CurrentFrameCommandContext.StateCache.ResetForNewCommandlist();

	GPUTimerBeginFrame(&CurrentFrameCommandContext);

	FrametimeGPUTimer.Start(CurrentFrameCommandContext.CommandQueueList[ED3D12QueueType::Direct], CurrentFrameCommandContext.GraphicsCommandList.get());

	D3D12Manager.OnStartFrame(CurrentFrameCommandContext);
	FImguiHelperSingleton::GetInstance()->NewFrame();
}

bool FRenderer::Draw()
{
	GCurrentRendererState = ERendererState::Draw;

	if (GCurrentFrameIndex == GNumBackBufferCount)
	{
		FD3D12ResourceAllocator::GetInstance()->ResourceUploadBatcher.FreeUnusedUploadBuffers(); // free upload buffers allocated at app launch. this is temporary workaround
	}
	
	return true;
}

void FRenderer::OnPreEndFrame()
{
	GCurrentRendererState = ERendererState::OnPreEndFrame;

	D3D12Manager.OnPreEndFrame(CurrentFrameCommandContext);

	FImguiHelperSingleton::GetInstance()->EndDraw(CurrentFrameCommandContext);
}

void FRenderer::OnPostEndFrame()
{
	GCurrentRendererState = ERendererState::OnPostEndFrame;

	D3D12Manager.OnPostEndFrame();
}

void FRenderer::OnEndFrame()
{
	GCurrentRendererState = ERendererState::OnEndFrame;

	D3D12Manager.OnEndFrame(CurrentFrameCommandContext);

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	// Indicate that the back buffer will be used as a render target.
	eastl::shared_ptr<FD3D12Texture2DResource>& TargetRenderTarget = SwapChain->GetRenderTarget(GCurrentBackbufferIndex);

	// Indicate that the back buffer will now be used to present.
	CD3DX12_RESOURCE_BARRIER ResourceBarrierB = CD3DX12_RESOURCE_BARRIER::Transition(TargetRenderTarget->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CurrentFrameCommandContext.GraphicsCommandList->ResourceBarrierBatcher.AddBarrier(ResourceBarrierB);

	FrametimeGPUTimer.End(CurrentFrameCommandContext.GraphicsCommandList.get());
	GPUTimerEndFrame(&CurrentFrameCommandContext);

	CurrentFrameCommandContext.FlushCommandList(ED3D12QueueType::Direct);

	FFrameResourceContainer& CurrentFrameContainer = GetCurrentFrameResourceContainer();

	SwapChain->Present(0);

	CurrentFrameContainer.FrameWorkEndFence->Signal(CurrentFrameCommandContext.CommandQueueList[ED3D12QueueType::Direct], false);
}

void FRenderer::Destroy()
{
	GCurrentRendererState = ERendererState::Destroying;

	GetPreviousFrameResourceContainer().FrameWorkEndFence->CPUWaitOnLastSignal();

	FImguiHelperSingleton::GetInstance()->OnDestory();

	D3D12Manager.OnDestory(CurrentFrameCommandContext);
}

void FRenderer::PrepareDraw(FD3D12CommandContext& InCommandContext)
{
	RenderScene.PrepareToCreateMeshDrawList(InCommandContext);
}

void FRenderer::Tick()
{
	if (!bEverSceneSetup)
	{
		SCOPED_CPU_TIMER(Renderer_SceneSetup)
		SCOPED_MEMORY_TRACE(Renderer_SceneSetup)
		SceneSetup();
		bEverSceneSetup = true;
	}

	{
		SCOPED_CPU_TIMER(Renderer_OnPreStartFrame)
		SCOPED_MEMORY_TRACE(Renderer_OnPreStartFrame)
		OnPreStartFrame();
	}
	{
		SCOPED_CPU_TIMER(Renderer_OnStartFrame)
		SCOPED_MEMORY_TRACE(Renderer_OnStartFrame)
		OnStartFrame();
	}
	{
		SCOPED_CPU_TIMER(Renderer_Draw)
		SCOPED_MEMORY_TRACE(Renderer_Draw)
		SCOPED_GPU_TIMER_DIRECT_QUEUE(CurrentFrameCommandContext, Renderer_Draw)
		Draw();
	}
	{
		SCOPED_CPU_TIMER(Renderer_OnPreEndFrame)
		SCOPED_MEMORY_TRACE(Renderer_OnPreEndFrame)
		OnPreEndFrame();
	}
	{
		SCOPED_CPU_TIMER(Renderer_OnEndFrame)
		SCOPED_MEMORY_TRACE(Renderer_OnEndFrame)
		OnEndFrame();
	}
	{
		SCOPED_CPU_TIMER(Renderer_OnPostEndFrame)
		SCOPED_MEMORY_TRACE(Renderer_OnPostEndFrame)
		OnPostEndFrame();
	}

}

FFrameResourceContainer& FRenderer::GetPreviousFrameResourceContainer()
{
	EA_ASSERT(GCurrentRendererState | FFrameResourceContainer::AccessibleRendererState);

	const uint64_t CurrentBackbufferIndex = GCurrentBackbufferIndex;
	return FrameContainerList[(CurrentBackbufferIndex == 0) ? (GNumBackBufferCount - 1) : (CurrentBackbufferIndex - 1)];
}

FFrameResourceContainer& FRenderer::GetCurrentFrameResourceContainer()
{
	EA_ASSERT(GCurrentRendererState | FFrameResourceContainer::AccessibleRendererState);
	
	return FrameContainerList[GCurrentBackbufferIndex];
}

eastl::array<FFrameResourceContainer, GNumBackBufferCount>& FRenderer::GetFrameContainerList()
{
	return FrameContainerList;
}

void FRenderer::SetUpGlobalConstantBuffer()
{
	// @todo Setup ViewConstantBuffer for draws

}
