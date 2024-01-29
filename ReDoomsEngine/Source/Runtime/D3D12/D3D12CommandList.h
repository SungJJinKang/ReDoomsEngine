#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"
#include "D3D12CommandQueue.h"

class D3D12CommandList
{
public:

	D3D12CommandList(const ED3D12QueueType InQueueType);
	void Init();

	ComPtr<ID3D12GraphicsCommandList> CommandList;

private:

	ED3D12QueueType QueueType;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};

