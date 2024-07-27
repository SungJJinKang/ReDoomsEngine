#pragma once
#include "CommonInclude.h"
#include "D3D12StateCache.h"

class FD3D12CommandQueue;
class FD3D12CommandAllocator;
class FD3D12CommandList;
class FD3D12StateCache;
class FFrameResourceContainer;
struct FD3D12CommandContext
{
	uint64_t TargetBackbufferIndex;
	FD3D12StateCache StateCache;
	FFrameResourceContainer* FrameResourceCounter = nullptr;
	eastl::array<FD3D12CommandQueue*, ED3D12QueueType::NumD3D12QueueType> CommandQueueList;
	eastl::array<eastl::shared_ptr<FD3D12CommandAllocator>, static_cast<uint32_t>(ECommandAllocatorType::Num)> CommandAllocatorList;
	eastl::shared_ptr<FD3D12CommandList> GraphicsCommandList;

	void DrawInstanced(
		uint32_t VertexCountPerInstance,
		uint32_t InstanceCount,
		uint32_t StartVertexLocation,
		uint32_t StartInstanceLocation);

	void DrawIndexedInstanced(
		uint32_t IndexCountPerInstance,
		uint32_t InstanceCount,
		uint32_t StartIndexLocation,
		int32_t BaseVertexLocation,
		uint32_t StartInstanceLocation);

	void Dispatch(
		uint32_t ThreadGroupCountX,
		uint32_t ThreadGroupCountY,
		uint32_t ThreadGroupCountZ);

	void FlushCommandList(const ED3D12QueueType InD3D12QueueType, const bool InWaitOnCompletation = false);
};

