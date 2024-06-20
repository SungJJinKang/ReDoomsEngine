#pragma once
#include "CommonInclude.h"

#include "D3D12Include.h"
#include "D3D12CommandQueue.h"
#include "D3D12Fence.h"
#include "Common/RendererStateCallbackInterface.h"
#include "D3D12ResourceBarrierBatcher.h"

class FD3D12CommandAllocator;
class FD3D12CommandQueue;
struct FD3D12PSO;
class FD3D12CommandList : public eastl::enable_shared_from_this<FD3D12CommandList>
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
	void FinishRecordingCommandList();

	void FlushResourceBarriers();

	FD3D12Fence Fence;
	FD3D12ResourceBarrierBatcher ResourceBarrierBatcher;

private:

	FD3D12CommandAllocator* const OwnerCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
};

class FD3D12CommandAllocator
{
public:

	FD3D12CommandAllocator() = delete;
	FD3D12CommandAllocator(const ED3D12QueueType InQueueType);

	void Init();

	inline ED3D12QueueType GetQueueType() const 
	{
		return QueueType;
	}

	ID3D12CommandAllocator* GetD3DCommandAllocator() const 
	{
		return CommandAllocator.Get();
	}

	eastl::shared_ptr<FD3D12CommandList> GetOrCreateNewCommandList();

	void ResetCommandAllocator(const bool bWaitForCompletation = true);

private:

	void InitCommandAllocator();

	bool bInit = false;

	ED3D12QueueType QueueType;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;

	eastl::queue<eastl::shared_ptr<FD3D12CommandList>> AllocatedCommandListPool;
};