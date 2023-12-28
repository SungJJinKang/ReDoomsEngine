#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Device;
class FD3D12Adapter
{
public:

	IDXGIAdapter1* GetD3D12Adapter() const;

	FD3D12Adapter(IDXGIFactory1& Factory, const bool bRequestHighPerformanceAdapter);
	void CreateDevice();

private:

	ComPtr<IDXGIAdapter1> Adapter;
	FD3D12Device* Device;
};

