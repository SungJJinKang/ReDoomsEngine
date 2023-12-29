#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12CommandQueue;
class FD3D12Window;
class FD3D12Swapchain
{
public:

	FD3D12Swapchain(FD3D12CommandQueue* const InCommandQueue, FD3D12Window* const InWindow, const uint32_t InNumBuffer, const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat);
	void Init();
	IDXGISwapChain1* GetD3DSwapchain()
	{
		return D3DSwapchain.Get();
	}

	void Present(const int32_t SyncInterval);

private:
	ComPtr<IDXGISwapChain1> D3DSwapchain;

	FD3D12CommandQueue* CommandQueue;
	FD3D12Window* Window;
	uint32_t NumBuffer;
	uint32_t Width;
	uint32_t Height;
	DXGI_FORMAT Format;
};

