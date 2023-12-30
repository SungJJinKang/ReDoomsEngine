#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Device;
class FD3D12CommandQueue;
class FD3D12Fence
{
public:

	void CreateD3DFence();
	void CreateD3DFence(const eastl::wstring& InDebugName);
	void SetDebugNameToFence(const eastl::wstring& InDebugName);
	ID3D12Fence* GetD3DFence() const
	{
		return D3DFence.Get();
	}
	uint64_t Signal(FD3D12CommandQueue* const InCommandQueue);

private:

	ComPtr<ID3D12Fence> D3DFence{};
	uint64_t LastSignaledValue = 0;
	bool bInterruptAwaited = false;
};

