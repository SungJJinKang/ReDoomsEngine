#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12PSO.h"

FD3D12CommandList::FD3D12CommandList(FD3D12CommandAllocator* const InOwnerCommandAllocator)
	: OwnerCommandAllocator(InOwnerCommandAllocator), Fence()
{

}

void FD3D12CommandList::InitCommandList()
{
	EA_ASSERT(CommandList == nullptr);
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandList(0, GetD3D12CommandListType(GetQueueType()), OwnerCommandAllocator->GetD3DCommandAllocator(), nullptr, IID_PPV_ARGS(&CommandList)));


	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	CommandList->Close();
}

ED3D12QueueType FD3D12CommandList::GetQueueType() const
{
	return OwnerCommandAllocator->GetQueueType();
}

void FD3D12CommandList::ResetRecordingCommandList(FD3D12PSO* const InInitialPSO)
{
	VERIFYD3D12RESULT(CommandList->Reset(OwnerCommandAllocator->GetD3DCommandAllocator(), InInitialPSO->PSOObject.Get()));
}

void FD3D12CommandList::FinishRecordingCommandList(FD3D12CommandQueue* const InCommandQueue)
{
	VERIFYD3D12RESULT(CommandList->Close());
	Fence.Signal(InCommandQueue, false);
}

void FD3D12CommandList::WaitOnCompletation()
{
	Fence.WaitOnLastSignal();
}

FD3D12CommandAllocator::FD3D12CommandAllocator(const ED3D12QueueType InQueueType)
	: QueueType(InQueueType), CommandAllocator(), FreedCommandListPool(), AllocatedCommandListPool()
{

}

void FD3D12CommandAllocator::InitCommandAllocator()
{
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandAllocator(GetD3D12CommandListType(QueueType), IID_PPV_ARGS(&CommandAllocator)));
}

FD3D12CommandList* FD3D12CommandAllocator::GetOrCreateNewCommandList()
{
	FD3D12CommandList* CommandList = nullptr;

	if (FreedCommandListPool.size() > 0)
	{
		eastl::unique_ptr<FD3D12CommandList> FreedCommandAllocator = eastl::move(FreedCommandListPool.front());
		FreedCommandListPool.pop();

		CommandList = AllocatedCommandListPool.emplace_back(eastl::move(FreedCommandAllocator)).get();
	}
	else
	{
		CommandList = AllocatedCommandListPool.emplace_back(eastl::make_unique<FD3D12CommandList>(this)).get();
		CommandList->InitCommandList();
	}

	return CommandList;
}

void FD3D12CommandAllocator::FreeCommandList(FD3D12CommandList* const InCommandList, FD3D12PSO* const InInitialPSO)
{
	bool bSuccess = false;

	for (size_t Index = 0; Index < AllocatedCommandListPool.size(); ++Index)
	{
		if (AllocatedCommandListPool[Index].get() == InCommandList)
		{
			InCommandList->ResetRecordingCommandList(InInitialPSO);

			FreedCommandListPool.emplace(eastl::move(AllocatedCommandListPool[Index]));
			AllocatedCommandListPool.erase(AllocatedCommandListPool.begin() + Index);
			bSuccess = true;
			break;
		}
	}

	EA_ASSERT(bSuccess);
}

void FD3D12CommandAllocator::ResetCommandAllocator(const bool bWaitForCompletationOfCommandLists)
{
	// 	Apps call Reset to re-use the memory that is associated with a command allocator.
	// 	From this call to Reset, the runtime and driver determine that the graphics processing unit(GPU) is no longer executing any command lists that have recorded commands with the command allocator.
	// 	Unlike ID3D12GraphicsCommandList::Reset, it is not recommended that you call Reset on the command allocator while a command list is still being executed.

	if (bWaitForCompletationOfCommandLists)
	{
		for (eastl::unique_ptr<FD3D12CommandList>& CommandList : AllocatedCommandListPool)
		{
			CommandList->WaitOnCompletation();
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
