#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"
#include "D3D12Fence.h"

enum ED3D12QueueType : uint64_t
{
	Direct = 0,
	Copy,
	Async,
	
	Count
};

D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType(ED3D12QueueType QueueType);
const wchar_t* GetD3D12QueueTypeString(ED3D12QueueType QueueType);

class FD3D12Device;
class FD3D12CommandQueue
{
public:

	FD3D12CommandQueue(const ED3D12QueueType InQueueType);
	void Init();
	void WaitForCompletion();
	ID3D12CommandQueue* GetD3DCommandQueue() const
	{
		EA_ASSERT(D3DCommandQueue.Get());
		return D3DCommandQueue.Get();
	}

private:

	ED3D12QueueType QueueType;
	ComPtr<ID3D12CommandQueue> D3DCommandQueue;

	FD3D12Fence Fence;
};

