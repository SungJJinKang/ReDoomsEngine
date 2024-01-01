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

	D3D12_RESOURCE_HEAP_TIER GetResourceHeapTier() const
	{
		return ResourceHeapTier;
	}
	D3D12_RESOURCE_BINDING_TIER GetResourceBindingTier() const
	{
		return ResourceBindingTier;
	}

	D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion() const
	{
		return RootSignatureVersion;
	}

private:

	ComPtr<ID3D12Device> D3DDevice;
	FD3D12Adapter* Adapter;

	D3D12_RESOURCE_HEAP_TIER ResourceHeapTier;
	D3D12_RESOURCE_BINDING_TIER ResourceBindingTier;
	D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion;

	eastl::vector<FD3D12CommandQueue> CommandQueueList;
};