#include "D3D12ConstantBufferRingBuffer.h"
#include "D3D12Resource.h"
#include "Renderer/Renderer.h"

static uint64_t GRingBufferSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT * 10000 * GNumBackBufferCount;

void FD3D12ConstantBufferRingBuffer::Init()
{
	ConstantBufferRingBuffer.ConstantBufferResource = eastl::make_shared<FD3D12ConstantBufferResource>(GRingBufferSize, true, nullptr, true, false, false);
	ConstantBufferRingBuffer.ConstantBufferResource->InitResource();
	ConstantBufferRingBuffer.ConstantBufferResource->CreateD3D12Resource();
	ConstantBufferRingBuffer.ConstantBufferResource->SetDebugNameToResource(EA_WCHAR("ConstantBuffer Dynamic RingBuffer"));
	
	for (uint32_t BackBufferIndex = 0; BackBufferIndex < GNumBackBufferCount; ++BackBufferIndex)
	{
		OffsetFromBaseWhenStartFrameList[BackBufferIndex] = ConstantBufferRingBuffer.ConstantBufferResource->GetBufferSize();
	}
	ConstantBufferRingBuffer.CurrentOffset = ConstantBufferRingBuffer.ConstantBufferResource->GetBufferSize();
}

FD3D12ConstantBufferBlock FD3D12ConstantBufferRingBuffer::Allocate(uint64_t InSize)
{
	EA_ASSERT(InSize > 0);
	EA_ASSERT(GCurrentRendererState | AllocatableRendererState);

	FD3D12ConstantBufferBlock Block;

	EA_ASSERT(IsAligned(ConstantBufferRingBuffer.CurrentOffset, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	
	InSize = Align(InSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	EA_ASSERT(InSize <= ConstantBufferRingBuffer.ConstantBufferResource->GetBufferSize());

	if (ConstantBufferRingBuffer.ConstantBufferResource->GetBufferSize() < ConstantBufferRingBuffer.CurrentOffset + InSize)
	{
		ConstantBufferRingBuffer.CurrentOffset = 0;
	}

#define EXHAUST_RING_BUFFER EA_ASSERT_MSG(false, "Exhausted constant buffer ring buffer. Increase GRingBufferSize")

	{
		uint64_t StartOffset = ConstantBufferRingBuffer.CurrentOffset;
		uint64_t EndOffset = ConstantBufferRingBuffer.CurrentOffset + InSize;

		for (uint32_t BackBufferIndex = 0; BackBufferIndex < GNumBackBufferCount; ++BackBufferIndex)
		{
			if ((BackBufferIndex == GCurrentBackbufferIndex) && !bAnyAllocedThisFrame)
			{
				if (StartOffset < OffsetFromBaseWhenStartFrameList[BackBufferIndex] && EndOffset > OffsetFromBaseWhenStartFrameList[BackBufferIndex])
				{
					EXHAUST_RING_BUFFER;
				}
			}
			else
			{
				if (StartOffset <= OffsetFromBaseWhenStartFrameList[BackBufferIndex] && EndOffset > OffsetFromBaseWhenStartFrameList[BackBufferIndex])
				{
					EXHAUST_RING_BUFFER;
				}
			}
		}
	}

	Block.OwnerResource = ConstantBufferRingBuffer.ConstantBufferResource.get();
	Block.GPUVirtualAddress = ConstantBufferRingBuffer.ConstantBufferResource->GPUVirtualAddress() + ConstantBufferRingBuffer.CurrentOffset;
	Block.MappedAddress = ConstantBufferRingBuffer.ConstantBufferResource->GetMappedAddress() + ConstantBufferRingBuffer.CurrentOffset;
	Block.SizeInBytes = InSize;
	Block.OffsetFromBase = ConstantBufferRingBuffer.CurrentOffset;

	ConstantBufferRingBuffer.CurrentOffset += InSize;

	bAnyAllocedThisFrame = true;

	EA_ASSERT_MSG(IsAligned(Block.GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), "GPUVirtualAddress should be aligned to 256 byte");
	return Block;
}

void FD3D12ConstantBufferRingBuffer::OnPreStartFrame()
{
	bAnyAllocedThisFrame = false;
	OffsetFromBaseWhenStartFrameList[GCurrentBackbufferIndex] = ConstantBufferRingBuffer.CurrentOffset;
}