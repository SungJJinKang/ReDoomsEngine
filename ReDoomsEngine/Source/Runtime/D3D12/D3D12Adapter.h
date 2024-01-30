#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Device;
class FD3D12Adapter
{
public:

	FD3D12Adapter(const bool bInRequestHighPerformanceAdapter);
	~FD3D12Adapter();

	void Init();

	FD3D12Device* GetDevice()
	{
		EA_ASSERT(Device);
		return Device;
	}

	IDXGIAdapter1* GetD3DAdapter()
	{
		EA_ASSERT(D3DAdapter.Get());
		return D3DAdapter.Get();
	}


private:

	ComPtr<IDXGIAdapter1> D3DAdapter;
	FD3D12Device* Device;

	bool bRequestHighPerformanceAdapter;
};

