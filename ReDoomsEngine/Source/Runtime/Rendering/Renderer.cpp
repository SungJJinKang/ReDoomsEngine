#include "Renderer.h"

FRenderer::FRenderer()
	: 
	D3D12Manager(true)
{
}

void FRenderer::Init()
{
	D3D12Manager.Init();

}

void FRenderer::OnStartFrame()
{
	FD3D12CommandAllocator* const CommandAllocator = FD3D12CommandListManager::GetInstance()->GetOrCreateNewCommandAllocator(ED3D12QueueType::Direct);
	FD3D12CommandList* const CommandList = CommandAllocator->GetOrCreateNewCommandList();
	CurrentFrameCommandContext.CommandList = CommandList;

	CommandAllocator->ResetCommandAllocator(false);
}

bool FRenderer::Draw()
{
	return true;
}

void FRenderer::OnEndFrame()
{
	FD3D12CommandQueue* const TargetCommandQueue = FD3D12Device::GetInstance()->GetCommandQueue(ED3D12QueueType::Direct);

	CurrentFrameCommandContext.CommandList->FinishRecordingCommandList(TargetCommandQueue);

	ID3D12CommandList* CommandLists[] = { CurrentFrameCommandContext.CommandList->GetD3DCommandList() };
	TargetCommandQueue->GetD3DCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);


	TargetCommandQueue->WaitForCompletion();

	FD3D12Swapchain* const SwapChain = FD3D12Manager::GetInstance()->GetSwapchain();
	SwapChain->Present(1);
	SwapChain->UpdateCurrentBackbufferIndex();
}

void FRenderer::Destroy()
{
}
