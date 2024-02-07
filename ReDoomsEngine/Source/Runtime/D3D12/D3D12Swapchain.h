#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "D3D12Resource/D3D12Resource.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12CommandQueue;
class FD3D12Window;
class FD3D12RenderTargetResource;
class FD3D12Swapchain : public ID3D12RendererStateCallbackInterface
{
public:

	FD3D12Swapchain(FD3D12CommandQueue* const InCommandQueue, FD3D12Window* const InWindow, const uint32_t InNumBuffer, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat);
	void Init();
	virtual void OnStartFrame();
	virtual void OnEndFrame();
	IDXGISwapChain3* GetD3DSwapchain() const
	{
		return D3DSwapchain.Get();
	}
	FD3D12RenderTargetResource& GetD3DRenderTargetResource(const uint32_t InBufferIndex);

	void Present(const int32_t SyncInterval);
	void UpdateCurrentBackbufferIndex();
	uint32_t GetCurrentBackbufferIndex() const
	{
		return CurrentBackbufferIndex;
	}
	FD3D12RenderTargetResource& GetRenderTarget(const size_t Index) 
	{
		return RenderTargets[Index];
	}

	uint32_t GetNumBuffer() const
	{
		return NumBuffer;
	}
	uint32_t GetWidth() const
	{
		return Width;
	}
	uint32_t GetHeight() const
	{
		return Height;
	}
	DXGI_FORMAT GetFormat() const
	{
		return Format;
	}

private:
	ComPtr<IDXGISwapChain3> D3DSwapchain;

	eastl::vector<FD3D12RenderTargetResource> RenderTargets;

	FD3D12CommandQueue* CommandQueue;
	FD3D12Window* Window;
	uint32_t NumBuffer;
	uint32_t Width;
	uint32_t Height;
	DXGI_FORMAT Format;

	uint32_t CurrentBackbufferIndex;
};

