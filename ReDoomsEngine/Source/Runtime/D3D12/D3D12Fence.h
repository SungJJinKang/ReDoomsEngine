#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Fence
{
public:
	ComPtr<ID3D12Fence> D3DFence;
	uint64_t LastSignaledValue = 0;
	bool bInterruptAwaited = false;
};

