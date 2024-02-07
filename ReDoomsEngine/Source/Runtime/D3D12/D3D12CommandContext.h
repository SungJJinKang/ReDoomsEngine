#pragma once
#include "CommonInclude.h"
#include "D3D12StateCache.h"

class FD3D12CommandQueue;
class FD3D12CommandAllocator;
class FD3D12CommandList;
struct FFrameResourceContainer;
struct FD3D12CommandContext
{
	FD3D12StateCache StateCache;
	FFrameResourceContainer* FrameResourceCounter = nullptr;
	eastl::shared_ptr<FD3D12CommandAllocator> GraphicsCommandAllocator;
	eastl::shared_ptr<FD3D12CommandList> GraphicsCommandList;
};

