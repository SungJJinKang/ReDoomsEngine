#include "imguiHelper.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "D3D12Window.h"
#include "D3D12Manager.h"
#include "D3D12Swapchain.h"
#include "D3D12Device.h"
#include "D3D12Descriptor.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandList.h"
#include "D3D12Window.h"

void FImguiHelper::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends

	SRVDescriptorHeapBlock = FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOnlineDescriptorHeapContainer.AllocatePermanentDescriptorHeapBlock(1);
	ImGui_ImplWin32_Init(FD3D12Window::GetInstance()->GetWindowHandle());
	ImGui_ImplDX12_Init(GetD3D12Device(), GNumBackBufferCount, FD3D12Manager::GetInstance()->GetSwapchain()->GetFormat(),
		FD3D12DescriptorHeapManager::GetInstance()->CbvSrvUavOnlineDescriptorHeapContainer.GetOnlineHeap()->D3DDescriptorHeap.Get(),
		// You'll need to designate a descriptor from your descriptor heap for Dear ImGui to use internally for its font texture's SRV
		SRVDescriptorHeapBlock.CPUDescriptorHandle(),
		SRVDescriptorHeapBlock.GPUDescriptorHandle());
}

void FImguiHelper::NewFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (eastl::function<void()>& imguiDrawEvent : ImguiDrawEventList)
	{
		imguiDrawEvent();
	}
}

void FImguiHelper::EndDraw(FD3D12CommandContext& InCommandContext)
{
	SCOPED_GPU_TIMER_DIRECT_QUEUE(InCommandContext, Imgui)

	InCommandContext.FlushResourceBarriers(EPipeline::Graphics);
	InCommandContext.StateCache.Flush(InCommandContext, EPipeline::Graphics);

	// Rendering
	// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), InCommandContext.GraphicsCommandList->GetD3DCommandList());
	// (Your code calls ExecuteCommandLists, swapchain's Present(), etc.)
}

void FImguiHelper::OnDestory()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}