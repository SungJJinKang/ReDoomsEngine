#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"
#include "D3D12CommandQueue.h"
#include "D3D12Fence.h"

class FD3D12CommandAllocator;
class FD3D12CommandQueue;
struct FD3D12PSO;
class FD3D12CommandList
{
public:

	FD3D12CommandList(FD3D12CommandAllocator* const InOwnerCommandAllocator);
	void InitCommandList();
	ED3D12QueueType GetQueueType() const;

	ID3D12GraphicsCommandList* GetD3DCommandList() const
	{
		return CommandList.Get();
	}

	void ResetRecordingCommandList(FD3D12PSO* const InInitialPSO);
	void FinishRecordingCommandList(FD3D12CommandQueue* const InCommandQueue);
	void WaitOnCompletation();

private:

	FD3D12CommandAllocator* const OwnerCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	FD3D12Fence Fence;
};

class FD3D12CommandAllocator
{
public:

	FD3D12CommandAllocator() = delete;
	FD3D12CommandAllocator(const ED3D12QueueType InQueueType);
	void InitCommandAllocator();

	inline ED3D12QueueType GetQueueType() const 
	{
		return QueueType;
	}

	ID3D12CommandAllocator* GetD3DCommandAllocator() const 
	{
		return CommandAllocator.Get();
	}

	FD3D12CommandList* GetOrCreateNewCommandList();
	void FreeCommandList(FD3D12CommandList* const InCommandList, FD3D12PSO* const InInitialPSO = nullptr);

	void ResetCommandAllocator(const bool bWaitForCompletation = true);

private:

	ED3D12QueueType QueueType;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;

	eastl::queue<eastl::unique_ptr<FD3D12CommandList>> FreedCommandListPool;
	eastl::vector<eastl::unique_ptr<FD3D12CommandList>> AllocatedCommandListPool;
};

class FD3D12CommandListManager : public EA::StdC::Singleton<FD3D12CommandListManager>
{
public:

	void Init();
	FD3D12CommandAllocator* GetOrCreateNewCommandAllocator(const ED3D12QueueType QueueType);
	void FreeCommandAllocator(FD3D12CommandAllocator* const InCommandAllocator);

private:

	eastl::array<eastl::queue<eastl::unique_ptr<FD3D12CommandAllocator>>, ED3D12QueueType::Count> FreedCommandAllocatorPool;
	eastl::array<eastl::vector<eastl::unique_ptr<FD3D12CommandAllocator>>, ED3D12QueueType::Count> AllocatedCommandAllocatorPool;
};
