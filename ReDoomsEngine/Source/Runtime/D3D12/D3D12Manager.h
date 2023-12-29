#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Window;
class FD3D12Adapter;
class FD3D12Swapchain;

class FD3D12Manager : public EA::StdC::Singleton<FD3D12Manager>
{
public:

	FD3D12Manager(const bool bInEnableDebugLayer);
	~FD3D12Manager();
	void Init();

	IDXGIFactory4* GetDXGIFactory()
	{
		EA_ASSERT(DXGIFactory.Get());
		return DXGIFactory.Get();
	}
	FD3D12Adapter* GetChoosenAdapter()
	{
		EA_ASSERT(ChoosenAdapter);
		return ChoosenAdapter;
	}
	FD3D12Swapchain* GetSwapchain()
	{
		EA_ASSERT(Swapchain);
		return Swapchain;
	}
	
private:

	ComPtr<IDXGIFactory4> DXGIFactory;

	FD3D12Window* D3D12Window;
	FD3D12Adapter* ChoosenAdapter;
	FD3D12Swapchain* Swapchain;

	bool bEnableDebugLayer;
};

