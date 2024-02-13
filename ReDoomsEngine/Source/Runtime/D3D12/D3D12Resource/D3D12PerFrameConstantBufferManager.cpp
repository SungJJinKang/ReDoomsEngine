#include "D3D12PerFrameConstantBufferManager.h"
#include "D3D12Resource.h"

static uint64_t GRingBufferSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT * 30;

void FD3D12PerFrameConstantBuffer::Reset()
{
	CurrentOffset = 0;
}

void FD3D12PerFrameConstantBufferManager::Init()
{
	for (FD3D12PerFrameConstantBuffer& RingBuffer : ConstantBufferRingBufferList)
	{
		RingBuffer.ConstantBufferResource = eastl::make_shared<FD3D12ConstantBufferResource>(GRingBufferSize, true, nullptr, true);
		RingBuffer.ConstantBufferResource->InitResource();
		RingBuffer.ConstantBufferResource->CreateD3D12Resource();
	}
}

FD3D12ConstantBufferBlock FD3D12PerFrameConstantBufferManager::Allocate(const uint64_t InSize)
{
	FD3D12ConstantBufferBlock Block;

	FD3D12PerFrameConstantBuffer& RingBuffer = ConstantBufferRingBufferList[GCurrentBackbufferIndex];
	EA_ASSERT(IsAligned(RingBuffer.CurrentOffset, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	
	const uint64_t AlignedSize = Align(InSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	// if exhaust resource, just assert!
	EA_ASSERT_MSG(RingBuffer.ConstantBufferResource->GetBufferSize() >= RingBuffer.CurrentOffset + InSize, "Exhausted ring buffer resource. Increase the ringbuffer size");

	Block.Resource = RingBuffer.ConstantBufferResource;
	Block.GPUVirtualAddress = RingBuffer.ConstantBufferResource->GPUVirtualAddress() + RingBuffer.CurrentOffset;
	Block.MappedAddress = RingBuffer.ConstantBufferResource->GetMappedAddress() + RingBuffer.CurrentOffset;
	Block.SizeInBytes = AlignedSize;
	Block.OffsetFromBase = RingBuffer.CurrentOffset;


	RingBuffer.CurrentOffset += InSize;

	EA_ASSERT_MSG(IsAligned(Block.GPUVirtualAddress, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), "GPUVirtualAddress should be aligned to 256 byte");
	return Block;
}

void FD3D12PerFrameConstantBufferManager::OnPreStartFrame()
{
}

void FD3D12PerFrameConstantBufferManager::OnStartFrame(FD3D12CommandContext& InCommandContext)
{
	FD3D12PerFrameConstantBuffer& RingBuffer = ConstantBufferRingBufferList[GCurrentBackbufferIndex];
	RingBuffer.Reset();
}

void FD3D12PerFrameConstantBufferManager::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}
