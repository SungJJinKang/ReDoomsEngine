#include "D3D12ResourcePool.h"

#include "D3D12Device.h"

void FD3D12ResourcePoolBlock::FreeBlock()
{
	if (eastl::shared_ptr<FD3D12ResourcePoolHeapContainer> ResourcePoolHeapContainer = OwnerResourcePoolHeapContainer.lock())
	{
		ResourcePoolHeapContainer->FreeBlockList.emplace_back(*this);
	}
	OwnerResourcePoolHeapContainer.reset();
}

bool FD3D12ResourcePool::IsForPlacedResource() const
{
	return HeapDesc.SizeInBytes != 0;
}

bool FD3D12ResourcePool::AllocateBlock(const uint64_t InSize, const uint64_t InAlignment, FD3D12ResourcePoolBlock& OutAllocatedBlock)
{

	bool bSuccess = false;

	const uint64_t AlignedSize = Align(InSize, InAlignment);
	EA_ASSERT(AlignedSize <= HeapDesc.SizeInBytes);

	auto TryAllocateBlock = [&OutAllocatedBlock, AlignedSize, &bSuccess, InAlignment](eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& ResourcePoolHeapContainer) {
		for (size_t FreeBlockIndex = 0; FreeBlockIndex < ResourcePoolHeapContainer->FreeBlockList.size(); ++FreeBlockIndex)
		{
			FD3D12ResourcePoolBlock& FreedBlock = ResourcePoolHeapContainer->FreeBlockList[FreeBlockIndex];

			uint64_t StartOffsetFromBaseOfFreeBlock = FreedBlock.OffsetFromBase;
			uint64_t EndOffsetFromBaseOfFreeBlock = FreedBlock.OffsetFromBase + FreedBlock.Size;

			uint64_t StartOffsetFromBase = Align(FreedBlock.OffsetFromBase, InAlignment);
			uint64_t EndOffsetFromBase = StartOffsetFromBase + AlignedSize;

			if (StartOffsetFromBaseOfFreeBlock <= StartOffsetFromBase &&
				EndOffsetFromBaseOfFreeBlock >= EndOffsetFromBase
			)
			{
				OutAllocatedBlock.OwnerResourcePoolHeapContainer = ResourcePoolHeapContainer;
				OutAllocatedBlock.OffsetFromBase = StartOffsetFromBase;
				OutAllocatedBlock.Size = AlignedSize;

				if (StartOffsetFromBaseOfFreeBlock < StartOffsetFromBase)
				{
					FD3D12ResourcePoolBlock NewFreedBlock{};

					NewFreedBlock.OwnerResourcePoolHeapContainer = ResourcePoolHeapContainer;
					NewFreedBlock.OffsetFromBase = StartOffsetFromBaseOfFreeBlock;
					NewFreedBlock.Size = StartOffsetFromBase - StartOffsetFromBaseOfFreeBlock;

					NewFreedBlock.FreeBlock();
				}
				if (EndOffsetFromBaseOfFreeBlock > EndOffsetFromBase)
				{
					FD3D12ResourcePoolBlock NewFreedBlock{};

					NewFreedBlock.OwnerResourcePoolHeapContainer = ResourcePoolHeapContainer;
					NewFreedBlock.OffsetFromBase = EndOffsetFromBase;
					NewFreedBlock.Size = EndOffsetFromBaseOfFreeBlock - EndOffsetFromBase;

					NewFreedBlock.FreeBlock();
				}

				ResourcePoolHeapContainer->FreeBlockList.erase(ResourcePoolHeapContainer->FreeBlockList.begin() + FreeBlockIndex);
				ResourcePoolHeapContainer->AllocatedBlockList.push_back(OutAllocatedBlock);

				bSuccess = true;
				break;
			}
		}
	};

	for (eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& ResourcePoolHeapContainer : ResourcePoolHeapContainerList)
	{
		TryAllocateBlock(ResourcePoolHeapContainer);

		if (bSuccess)
		{
			break;
		}
	}

	if (!bSuccess)
	{
		eastl::shared_ptr<FD3D12ResourcePoolHeapContainer> NewResourcePoolHeapContainer = AllocateNewResourcePoolHeapContainer();
		TryAllocateBlock(NewResourcePoolHeapContainer);
	}

	EA_ASSERT(bSuccess);

	return bSuccess;
}

eastl::shared_ptr<FD3D12ResourcePoolHeapContainer> FD3D12ResourcePool::AllocateNewResourcePoolHeapContainer()
{
	eastl::shared_ptr<FD3D12ResourcePoolHeapContainer>& NewResourcePoolHeapContainer = ResourcePoolHeapContainerList.emplace_back(eastl::make_shared<FD3D12ResourcePoolHeapContainer>());
	VERIFYD3D12RESULT(GetD3D12Device()->CreateHeap(&(HeapDesc), IID_PPV_ARGS(&(NewResourcePoolHeapContainer->Heap))));

	FD3D12ResourcePoolBlock FreeBlock{};
	FreeBlock.Size = HeapDesc.SizeInBytes;
	FreeBlock.OffsetFromBase = 0;

	NewResourcePoolHeapContainer->FreeBlockList.emplace_back(FreeBlock);

	return NewResourcePoolHeapContainer;
}
