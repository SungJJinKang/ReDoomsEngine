#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Device;
class FD3D12CommandQueue;

// @TODO : Implement fence pool
class FD3D12Fence
{
public:

	FD3D12Fence(const bool bInit = false);
	~FD3D12Fence();
	void InitIfRequired();
	void InitIfRequired(const wchar_t* const InDebugName);
	void Release();
	#if D3D_NAME_OBJECT
	void SetDebugNameToFence(const wchar_t* const InDebugName);
	#else
	void SetDebugNameToFence(const wchar_t* const InDebugName) {}
	#endif

	inline bool IsInit() const
	{
		return D3DFence.Get();
	}
	ID3D12Fence* GetD3DFence() const
	{
		return D3DFence.Get();
	}
	uint64_t Signal(FD3D12CommandQueue* const InCommandQueue, const bool bWaitInstantly = false);
	void CPUWaitOnSignal(const uint64_t SignaledValue);
	void CPUWaitOnLastSignal();

	/*
		https://gpuopen.com/learn/rdna-performance-guide/#synchronization
		Work submitted does not need semaphores/fences for synchronization on the same queue – use barriers in the command buffers.
		Command buffers will be executed in order of submission.
		Cross queue synchronization is done with fences or semaphores at submission boundaries.
	*/
	void GPUWaitOnSignal(FD3D12CommandQueue* const InCommandQueue, const uint64_t SignaledValue);
	void GPUWaitOnLastSignal(FD3D12CommandQueue* const InCommandQueue);
	bool IsCompleteSignal(const uint64_t SignaledValue);
	bool IsCompleteLastSignal();

private:

	void CreateD3DFence();
	void CreateD3DFence(const wchar_t* const InDebugName);

	ComPtr<ID3D12Fence> D3DFence{};
	uint64_t LastSignaledValue = 0;
	bool bInterruptAwaited = false;
};

