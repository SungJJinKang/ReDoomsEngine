#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12ManagerInterface.h"

class FD3D12Window;
class FD3D12Adapter;
class FD3D12Swapchain;
class FD3D12DescriptorHeapManager;
class FD3D12ShaderManager;
class FD3D12PSOManager;
class FD3D12RootSignatureManager;
class FD3D12CommandListManager;
class FD3D12ConstantBufferRingBufferManager;

class FD3D12Manager : public EA::StdC::Singleton<FD3D12Manager>, public ID3D12ManagerInterface
{
public:

	FD3D12Manager();
	FD3D12Manager(FD3D12Manager&&);
	FD3D12Manager& operator=(FD3D12Manager&&);
	~FD3D12Manager();
	void Init();
	virtual void OnPreStartFrame();
	virtual void OnStartFrame();
	virtual void OnEndFrame();
	virtual void OnPostEndFrame();


	IDXGIFactory4* GetDXGIFactory() const
	{
		EA_ASSERT(DXGIFactory.Get());
		return DXGIFactory.Get();
	}
	FD3D12Adapter* GetChoosenAdapter() const
	{
		EA_ASSERT(ChoosenAdapter);
		return ChoosenAdapter.get();
	}
	FD3D12Swapchain* GetSwapchain() const
	{
		EA_ASSERT(Swapchain);
		return Swapchain.get();
	}
	
private:

	ComPtr<IDXGIFactory4> DXGIFactory;

	eastl::unique_ptr<FD3D12Window> D3D12Window;
	eastl::unique_ptr<FD3D12Adapter> ChoosenAdapter;
	eastl::unique_ptr<FD3D12Swapchain> Swapchain;
	eastl::unique_ptr<FD3D12DescriptorHeapManager> D3D12DescriptorHeapManager;
	eastl::unique_ptr<FD3D12ShaderManager> D3D12ShaderManager;
	eastl::unique_ptr<FD3D12PSOManager> D3D12PSOManager;
	eastl::unique_ptr<FD3D12RootSignatureManager> D3D12RootSignatureManager;
	eastl::unique_ptr<FD3D12CommandListManager> D3D12CommandListManager;
	eastl::unique_ptr<FD3D12ConstantBufferRingBufferManager> D3D12ConstantBufferRingBufferManager;

	eastl::vector<ID3D12ManagerInterface*> TickedManagerList;
};

