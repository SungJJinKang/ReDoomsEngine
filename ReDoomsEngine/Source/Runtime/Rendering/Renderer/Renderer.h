#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"
#include "D3D12Window.h"
#include "RendererCommonInclude.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "Common/RendererStateCallbackInterface.h"
#include "D3D12Shader.h"
#include "View.h"
#include "RenderScene.h"
#include "Thread/JobSystem.h"

class FD3D12CommandAllocator;

/// <summary>
/// Contains datas about a frame
/// </summary>
class FFrameResourceContainer
{
public:
	eastl::array<eastl::shared_ptr<FD3D12CommandAllocator>, static_cast<uint32_t>(ECommandAllocatorType::Num)> CommandAllocatorList;
	eastl::shared_ptr<FD3D12Fence> FrameWorkEndFence;
	eastl::vector<eastl::shared_ptr<FD3D12Fence>> TransientFrameWorkEndFenceList;
	eastl::vector<eastl::weak_ptr<FD3D12Resource>> DeferredDeletedResourceList;

	void Init(eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> InOnlineDescriptorHeap);
	void ResetForNewFrame();

	static constexpr uint64_t AccessibleRendererState = ERendererState::OnStartFrame | ERendererState::Draw | ERendererState::OnPreEndFrame | ERendererState::OnEndFrame;

private:

	bool bInit = false;
};

class FRenderer : public EA::StdC::Singleton<FRenderer>
{
public:

	FRenderer() = default;
	virtual ~FRenderer() = default;
	virtual void Init();

	/// <summary>
	/// This function is called once just after start scene
	/// </summary>
	virtual void SceneSetup();
	virtual void OnPreStartFrame();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void OnPreEndFrame();
	void OnEndFrame();
	virtual void OnPostEndFrame();
	virtual void Destroy();

	virtual void PrepareDraw(FD3D12CommandContext& InCommandContext);

	void Tick();

	FFrameResourceContainer& GetPreviousFrameResourceContainer();
	FFrameResourceContainer& GetCurrentFrameResourceContainer();
	eastl::array<FFrameResourceContainer, GNumBackBufferCount>& GetFrameContainerList();

	void SetUpGlobalConstantBuffer();

	FJobSystem JobSystem;
	FD3D12Manager D3D12Manager;

	FRenderScene RenderScene;
	FView View;

protected:

	FD3D12CommandContext CurrentFrameCommandContext;

	eastl::shared_ptr<FD3D12VertexIndexBufferResource> VertexBuffer;

	eastl::array<FFrameResourceContainer, GNumBackBufferCount> FrameContainerList;

private :

	bool bEverSceneSetup = false;
	FGPUTimer FrametimeGPUTimer{ "FrameTime" };
};