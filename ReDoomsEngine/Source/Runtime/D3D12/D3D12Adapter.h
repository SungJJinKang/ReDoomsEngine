#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12Device;
class FD3D12Adapter : public ID3D12RendererStateCallbackInterface
{
public:

	FD3D12Adapter(const bool bInRequestHighPerformanceAdapter);
	~FD3D12Adapter();

	void Init();
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

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

