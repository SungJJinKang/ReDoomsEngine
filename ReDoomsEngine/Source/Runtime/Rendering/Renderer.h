#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"
#include "D3D12Window.h"
#include "D3D12RendererInclude.h"
#include "D3D12Fence.h"

class FD3D12CommandAllocator;

/// <summary>
/// Contains datas about a frame
/// </summary>
struct FFrameResourceContainer
{
	FD3D12CommandAllocator* GraphicsCommandAllocator;
	FD3D12CommandList* GraphicsCommandList; // currently support only graphcis command list
	FD3D12Fence FrameWorkEndFence;

	void Init();
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

	FFrameResourceContainer& GetCurrentFrameContainer();

	FD3D12Manager D3D12Manager;

protected:

	FD3D12CommandContext CurrentFrameCommandContext;

	eastl::unique_ptr<FD3D12BufferResource> VertexBuffer;
	uint32_t VerticeStride;

	eastl::array<FFrameResourceContainer, GNumBackBufferCount> FrameContainerList;
};
