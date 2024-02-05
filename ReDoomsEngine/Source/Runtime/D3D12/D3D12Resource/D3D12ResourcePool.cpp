#include "D3D12ResourcePool.h"

bool FD3D12ResourcePool::IsForPlacedResource() const
{
	return HeapDesc.SizeInBytes != 0;
}

bool FD3D12ResourcePool::AllocateBlock(const uint64_t InSize, const uint64_t InAlignment, FD3D12ResourcePoolBlock& OutAllocatedBlock)
{
	bool bSuccess = false;

	const uint64_t AlignedSize = Align(InSize, InAlignment);

	for (eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& ResourcePoolHeapContainer : ResourcePoolHeapContainerList)
	{
		for (size_t FreeBlockIndex = 0; FreeBlockIndex < ResourcePoolHeapContainer->FreeBlockList.size(); ++FreeBlockIndex)
		{
			FD3D12ResourcePoolBlock& FreedBlock = ResourcePoolHeapContainer->FreeBlockList[FreeBlockIndex];
			if (FreedBlock.Size >= AlignedSize)
			{
				OutAllocatedBlock.OwnerResourcePoolHeapContainer = ResourcePoolHeapContainer;
				OutAllocatedBlock.Size = AlignedSize;
				OutAllocatedBlock.OffsetFromBase = FreedBlock.OffsetFromBase;
				EA_ASSERT(IsAligned(OutAllocatedBlock.OffsetFromBase, HeapDesc.Alignment));

				if (FreedBlock.Size > AlignedSize)
				{
					FD3D12ResourcePoolBlock NewFreedBlock{};

					NewFreedBlock.OwnerResourcePoolHeapContainer = ResourcePoolHeapContainer;
					NewFreedBlock.Size = FreedBlock.Size - AlignedSize;
					NewFreedBlock.OffsetFromBase = FreedBlock.OffsetFromBase + AlignedSize;

					FreeBlock(NewFreedBlock);
				}

				ResourcePoolHeapContainer->FreeBlockList.erase(ResourcePoolHeapContainer->FreeBlockList.begin() + FreeBlockIndex);

				bSuccess = true;
				break;
			}
		}
	}

	return bSuccess;
}

void FD3D12ResourcePool::FreeBlock(const FD3D12ResourcePoolBlock& FreedBlock)
{
	FreedBlock.OwnerResourcePoolHeapContainer.lock()->FreeBlockList.emplace_back(FreedBlock);

	// @todo merge with adjacent block
}