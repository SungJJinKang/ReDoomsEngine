#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12CommandQueue.h"

class FD3D12Adapter;

class FD3D12Device : public EA::StdC::Singleton<FD3D12Device>
{
public:

	FD3D12Device(FD3D12Adapter* const InAdapter);

	void Init();

	FD3D12Adapter* GetAdapter() const
	{
		return Adapter;
	}

	ID3D12Device* GetD3D12Device() const
	{
		return D3DDevice.Get();
	}

	FD3D12CommandQueue* GetCommandQueue(const ED3D12QueueType QueueType)
	{
		return &CommandQueueList[static_cast<uint64_t>(QueueType)];
	}


private:

	ComPtr<ID3D12Device> D3DDevice;
	FD3D12Adapter* Adapter;

	eastl::vector<FD3D12CommandQueue> CommandQueueList;
};