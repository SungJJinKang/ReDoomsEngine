#pragma once
#include "CommonInclude.h"
#include "D3D12Device.h"

class FD3D12Manager
{
public:

	FD3D12Manager();
	void Init(const bool bEnableDebugLayer);

	IDXGIFactory4* GetDXGIFactory();
	
private:

	ComPtr<IDXGIFactory4> DXGIFactory;
	FD3D12Adapter* ChoosenAdapter;
};

