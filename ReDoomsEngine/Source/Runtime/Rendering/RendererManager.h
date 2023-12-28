#pragma once
#include "CommonInclude.h"
#include "D3D12Manager.h"

class FRendererManager
{
public:

	FRendererManager();
	void Init();

	FD3D12Manager D3D12Manager;
};

