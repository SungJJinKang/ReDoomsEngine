#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12PSO.h"

FD3D12CommandList::FD3D12CommandList(FD3D12CommandAllocator* const InOwnerCommandAllocator)
	: OwnerCommandAllocator(InOwnerCommandAllocator), Fence(false)
{

}

void FD3D12CommandList::InitCommandList()
{
	Fence.Init();

	EA_ASSERT(CommandList == nullptr);
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandList(0, GetD3D12CommandListType(GetQueueType()), OwnerCommandAllocator->GetD3DCommandAllocator(), nullptr, IID_PPV_ARGS(&CommandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	CommandList->Close();
	ResetRecordingCommandList(nullptr);
	OwnerCommandAllocator->FreeCommandList(shared_from_this());
}

ED3D12QueueType FD3D12CommandList::GetQueueType() const
{
	return OwnerCommandAllocator->GetQueueType();
}

void FD3D12CommandList::ResetRecordingCommandList(FD3D12PSO* const InInitialPSO)
{
	// By using Reset, you can re-use command list tracking structures without any allocations. 
	// Unlike ID3D12CommandAllocator::Reset, you can call Reset while the command list is still being executed. 
	// A typical pattern is to submit a command list and then immediately reset it to reuse the allocated memory for another command list.
	VERIFYD3D12RESULT(CommandList->Reset(OwnerCommandAllocator->GetD3DCommandAllocator(), InInitialPSO ? InInitialPSO->PSOObject.Get() : nullptr));
}

void FD3D12CommandList::FinishRecordingCommandList(FD3D12CommandQueue* const InCommandQueue)
{
	VERIFYD3D12RESULT(CommandList->Close());
}

FD3D12CommandAllocator::FD3D12CommandAllocator(const ED3D12QueueType InQueueType)
	: QueueType(InQueueType), CommandAllocator(), FreedCommandListPool(), AllocatedCommandListPool()
{

}

void FD3D12CommandAllocator::InitCommandAllocator()
{
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandAllocator(GetD3D12CommandListType(QueueType), IID_PPV_ARGS(&CommandAllocator)));
}

eastl::shared_ptr<FD3D12CommandList> FD3D12CommandAllocator::GetOrCreateNewCommandList()
{
	eastl::shared_ptr<FD3D12CommandList> CommandList{};

	for (size_t FreedCommandListIndex = 0; FreedCommandListIndex < FreedCommandListPool.size(); ++FreedCommandListIndex)
	{
		if (FreedCommandListPool[FreedCommandListIndex]->Fence.IsCompleteLastSignal())
		{
			CommandList = FreedCommandListPool[FreedCommandListIndex];
			FreedCommandListPool.erase(FreedCommandListPool.begin() + FreedCommandListIndex);
			break;
		}
	}

	if(!CommandList)
	{
		CommandList = AllocatedCommandListPool.emplace_back(eastl::make_shared<FD3D12CommandList>(this));
		CommandList->InitCommandList();
	}

	return CommandList;
}

void FD3D12CommandAllocator::FreeCommandList(eastl::shared_ptr<FD3D12CommandList> InCommandList)
{
	EA_ASSERT(eastl::find(FreedCommandListPool.begin(), FreedCommandListPool.end(), InCommandList) == FreedCommandListPool.end());
	FreedCommandListPool.emplace_back(InCommandList);
}

void FD3D12CommandAllocator::ResetCommandAllocator(const bool bWaitForCompletationOfCommandLists)
{
	// 	Apps call Reset to re-use the memory that is associated with a command allocator.
	// 	From this call to Reset, the runtime and driver determine that the graphics processing unit(GPU) is no longer executing any command lists that have recorded commands with the command allocator.
	// 	Unlike ID3D12GraphicsCommandList::Reset, it is not recommended that you call Reset on the command allocator while a command list is still being executed.

 	if (bWaitForCompletationOfCommandLists)
 	{
 		for (eastl::shared_ptr<FD3D12CommandList>& CommandList : AllocatedCommandListPool)
 		{
 			CommandList->Fence.WaitOnLastSignal();
 		}
 	}

	VERIFYD3D12RESULT(CommandAllocator->Reset());

}

void FD3D12CommandListManager::Init()
{

}

FD3D12CommandAllocator* FD3D12CommandListManager::GetOrCreateNewCommandAllocator(const ED3D12QueueType QueueType)
{
	FD3D12CommandAllocator* CommandAllocator = nullptr;

	if (FreedCommandAllocatorPool[QueueType].size() > 0)
	{
		eastl::unique_ptr<FD3D12CommandAllocator> FreedCommandAllocator = eastl::move(FreedCommandAllocatorPool[QueueType].front());
		FreedCommandAllocatorPool[QueueType].pop();

		CommandAllocator = AllocatedCommandAllocatorPool[QueueType].emplace_back(eastl::move(FreedCommandAllocator)).get();
	}
	else
	{
		CommandAllocator = AllocatedCommandAllocatorPool[QueueType].emplace_back(eastl::make_unique<FD3D12CommandAllocator>(QueueType)).get();
		CommandAllocator->InitCommandAllocator();
	}

	return CommandAllocator;
}

void FD3D12CommandListManager::FreeCommandAllocator(FD3D12CommandAllocator* const InCommandAllocator)
{
	bool bSuccess = false;

	eastl::vector<eastl::unique_ptr<FD3D12CommandAllocator>>& CommandAllocatorList = AllocatedCommandAllocatorPool[InCommandAllocator->GetQueueType()];

	for (size_t Index = 0 ; Index < CommandAllocatorList.size() ; ++Index)
	{
		if (CommandAllocatorList[Index].get() == InCommandAllocator)
		{
			FreedCommandAllocatorPool[InCommandAllocator->GetQueueType()].emplace(eastl::move(CommandAllocatorList[Index]));
			CommandAllocatorList.erase(CommandAllocatorList.begin() + Index);
			bSuccess = true;
			break;
		}
	}

	EA_ASSERT(bSuccess);
}

void FD3D12CommandListManager::OnStartFrame()
{

}

void FD3D12CommandListManager::OnEndFrame()
{

}
