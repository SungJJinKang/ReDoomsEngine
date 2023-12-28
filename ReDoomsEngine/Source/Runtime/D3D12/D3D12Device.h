#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12CommandQueue.h"

class FD3D12Adapter;

class FD3D12Device : public EA::StdC::Singleton<FD3D12Device>
{
public:

	FD3D12Adapter* GetAdapter();
	ID3D12Device* GetD3D12Device();
	FD3D12CommandQueue& GetCommandQueue(const ED3D12QueueType QueueType);

	FD3D12Device(FD3D12Adapter* const InAdapter, const bool bEnableDebugLayer);

private:

	ComPtr<ID3D12Device> Device;
	FD3D12Adapter* Adapter;

	eastl::vector<FD3D12CommandQueue> CommandQueue;
};

FD3D12Device* GetD3D12Device();
FD3D12Adapter* GetD3D12ChosenAdapter();