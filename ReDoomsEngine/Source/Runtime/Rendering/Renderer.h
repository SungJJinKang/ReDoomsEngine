#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"
#include "D3D12Window.h"
#include "RendererCommonInclude.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12CommandAllocator;

enum class ECommandAllocatotrType : uint32_t
{
	Graphics,
	ResourceUploadBatcher,

	Num
};

/// <summary>
/// Contains datas about a frame
/// </summary>
class FFrameResourceContainer
{
public:
	eastl::array<eastl::shared_ptr<FD3D12CommandAllocator>, static_cast<uint32_t>(ECommandAllocatotrType::Num)> CommandAllocatorList;
	FD3D12Fence FrameWorkEndFence;

	void Init(eastl::shared_ptr<FD3D12OnlineDescriptorHeapContainer> InOnlineDescriptorHeap);
	void ResetForNewFrame();

private:

	bool bInit = false;
};

enum class ERendererState
{
	Initializing,
	FinishInitialzing,
	OnPreStartFrame,
	OnStartFrame,
	Draw,
	OnEndFrame,
	OnPostEndFrame,
	Destroying
};

class FRenderer : public EA::StdC::Singleton<FRenderer>
{
public:

	FRenderer() = default;
	virtual ~FRenderer() = default;
	virtual void Init();
	virtual void OnPreStartFrame();
	virtual void OnStartFrame();
	virtual bool Draw();
	virtual void OnEndFrame();
	virtual void OnPostEndFrame();
	virtual void Destroy();

	FFrameResourceContainer& GetLastFrameContainer();
	FFrameResourceContainer& GetCurrentFrameContainer();

	inline ERendererState GetCurrentRendererState() const
	{
		return CurrentRendererState;
	}

	FD3D12Manager D3D12Manager;

protected:

	FD3D12CommandContext CurrentFrameCommandContext;

	eastl::unique_ptr<FD3D12BufferResource> VertexBuffer;
	uint32_t VerticeStride;

	eastl::array<FFrameResourceContainer, GNumBackBufferCount> FrameContainerList;

private :
	ERendererState CurrentRendererState = ERendererState::Initializing;
};