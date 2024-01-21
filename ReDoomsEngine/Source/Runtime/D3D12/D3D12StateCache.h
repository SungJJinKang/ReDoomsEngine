#pragma once
#include "D3D12PSO.h"

class FD3D12StateCache
{
	FD3D12PSOInitializer PSOInitializer;

	void Flush();
};

