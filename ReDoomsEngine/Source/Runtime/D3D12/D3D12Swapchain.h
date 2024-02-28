#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

#include "D3D12Resource/D3D12Resource.h"
#include "Common/RendererStateCallbackInterface.h"

class FD3D12CommandQueue;
class FD3D12Window;
class FD3D12Texture2DResource;
class FD3D12Swapchain : public IRendererStateCallbackInterface
{
public:

	FD3D12Swapchain(
		FD3D12CommandQueue* const InCommandQueue,
		FD3D12Window* const InWindow, 
		const uint32_t InNumBuffer, 
		const uint32_t InWidth, 
		const uint32_t InHeight, 
		const DXGI_FORMAT InFormat,
		const uint32_t InSampleCount, 
		const uint32_t InSampleQuality
	);
	void Init();
	bool IsRequireResize() const;
	void QueueResize(const uint32_t InWidth, const uint32_t InHeight);
	virtual void OnPreStartFrame();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);
	void ResizeIfRequired();
	IDXGISwapChain3* GetD3DSwapchain() const
	{
		return D3DSwapchain.Get();
	}
	void Present(const int32_t SyncInterval);
	void UpdateCurrentBackbufferIndex();
	eastl::shared_ptr<FD3D12Texture2DResource>& GetRenderTarget(const size_t Index)
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

	void CreateRenderTargets();

	ComPtr<IDXGISwapChain3> D3DSwapchain;

	eastl::vector<eastl::shared_ptr<FD3D12Texture2DResource>> RenderTargets;

	FD3D12CommandQueue* CommandQueue;
	FD3D12Window* Window;
	uint32_t NumBuffer;
	uint32_t Width;
	uint32_t Height;
	uint32_t ResizedWidth;
	uint32_t ResizedHeight;
	DXGI_FORMAT Format;
	uint32_t SampleCount;
	uint32_t SampleQuality;
};

