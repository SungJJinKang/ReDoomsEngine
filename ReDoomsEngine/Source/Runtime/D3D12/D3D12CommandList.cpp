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
	Fence.InitIfRequired();

	EA_ASSERT(CommandList == nullptr);
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandList(0, GetD3D12CommandListType(GetQueueType()), OwnerCommandAllocator->GetD3DCommandAllocator(), nullptr, IID_PPV_ARGS(&CommandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	FinishRecordingCommandList();
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

void FD3D12CommandList::FinishRecordingCommandList()
{
	VERIFYD3D12RESULT(CommandList->Close());
}

void FD3D12CommandList::FlushResourceBarriers()
{
	ResourceBarrierBatcher.Flush(*this);
}

FD3D12CommandAllocator::FD3D12CommandAllocator(const ED3D12QueueType InQueueType)
	: QueueType(InQueueType), CommandAllocator(), AllocatedCommandListPool()
{

}

void FD3D12CommandAllocator::Init()
{
	InitCommandAllocator();
	bInit = true;
}

void FD3D12CommandAllocator::InitCommandAllocator()
{
	VERIFYD3D12RESULT(GetD3D12Device()->CreateCommandAllocator(GetD3D12CommandListType(QueueType), IID_PPV_ARGS(&CommandAllocator)));
}

eastl::shared_ptr<FD3D12CommandList> FD3D12CommandAllocator::GetOrCreateNewCommandList()
{
	EA_ASSERT(bInit);

	eastl::shared_ptr<FD3D12CommandList> CommandList{};

	if (AllocatedCommandListPool.size() > 0)
	{
		if (AllocatedCommandListPool.front()->Fence.IsCompleteLastSignal())
		{
			CommandList = AllocatedCommandListPool.front();
			AllocatedCommandListPool.pop();
		}
	}

	if(!CommandList)
	{
		CommandList = eastl::make_shared<FD3D12CommandList>(this);
		CommandList->InitCommandList();
	}
	AllocatedCommandListPool.push(CommandList);
	CommandList->ResetRecordingCommandList(nullptr);

	return CommandList;
}

void FD3D12CommandAllocator::ResetCommandAllocator(const bool bWaitForCompletationOfCommandLists)
{
	EA_ASSERT(bInit);

	// 	Apps call Reset to re-use the memory that is associated with a command allocator.
	// 	From this call to Reset, the runtime and driver determine that the graphics processing unit(GPU) is no longer executing any command lists that have recorded commands with the command allocator.
	// 	Unlike ID3D12GraphicsCommandList::Reset, it is not recommended that you call Reset on the command allocator while a command list is still being executed.

 	if (bWaitForCompletationOfCommandLists && (AllocatedCommandListPool.size() > 0))
 	{
		AllocatedCommandListPool.back()->Fence.CPUWaitOnLastSignal();
 	}

	VERIFYD3D12RESULT(CommandAllocator->Reset());

}