#include "CullingModule.h"

#include "../EveryCulling.h"

culling::EntityBlock* culling::CullingModule::GetNextEntityBlock(const size_t cameraIndex)
{
	// TODO : Implement Cache Friendly GetNextEntityBlock to prevent cache coherency.
	//        After thread finished its blocks, steal other threads's block. 
	// Ex)
	// First Thread : 1 4 7 10  ....
	// Second Thread : 2 5 8 11 ....
	// Third Thread : 3 6 9 12 ....
	//

	const uint32_t currentEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndex[cameraIndex].fetch_add(1);

	const size_t entityBlockCount = mCullingSystem->GetActiveEntityBlockCount();
	EntityBlock* const currentEntityBlock = (currentEntityBlockIndex >= entityBlockCount) ? (nullptr) : (mCullingSystem->GetActiveEntityBlockList()[currentEntityBlockIndex]);

	return currentEntityBlock;
}

size_t culling::CullingModule::ComputeEndEntityBlockIndexOfThread(const int32_t threadIndex)
{
	return mCullingSystem->GetActiveEntityBlockCount() - (mCullingSystem->GetActiveEntityBlockCount() % threadIndex);
}

culling::CullingModule::CullingModule
(
	EveryCulling* cullingSystem
)
:mCullingSystem{ cullingSystem }, IsEnabled(true)
{

}

culling::CullingModule::~CullingModule() = default;

void culling::CullingModule::ResetCullingModule(const unsigned long long currentTickCount)
{
	for (eastl::atomic<uint32_t>& atomicVal : mCullJobState.mCurrentCulledEntityBlockIndex)
	{
		atomicVal.store(0, eastl::memory_order_relaxed);
	}
	

	for (eastl::atomic<uint32_t>& atomicVal : mCullJobState.mFinishedThreadCount)
	{
		atomicVal.store(0, eastl::memory_order_relaxed);
	}
}

void culling::CullingModule::ThreadCullJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
	eastl::atomic_thread_fence(eastl::memory_order_acquire);
	CullBlockEntityJob(cameraIndex, currentTickCount);


	mCullJobState.mFinishedThreadCount[cameraIndex].fetch_add(1, eastl::memory_order_seq_cst);
}
