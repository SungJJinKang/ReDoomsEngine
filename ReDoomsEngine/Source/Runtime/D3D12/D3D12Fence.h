#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Device;
class FD3D12CommandQueue;
class FD3D12Fence
{
public:

	FD3D12Fence();
	FD3D12Fence(FD3D12Device* const InDevice);
	FD3D12Fence(FD3D12Device* const InDevice, const eastl::wstring& InDebugName);

	void CreateD3DFence(FD3D12Device* const InDevice);
	void SetDebugNameToFence(const eastl::wstring& InDebugName);
	ID3D12Fence* GetD3DFence()
	{
		return D3DFence.Get();
	}
	uint64_t Signal(FD3D12CommandQueue* const InCommandQueue);

private:

	ComPtr<ID3D12Fence> D3DFence{};
	uint64_t LastSignaledValue = 0;
	bool bInterruptAwaited = false;
};

