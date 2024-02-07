#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12Window;
class FD3D12Adapter;
class FD3D12Swapchain;
class FD3D12DescriptorHeapManager;
class FD3D12ShaderManager;
class FD3D12PSOManager;
class FD3D12RootSignatureManager;
class FD3D12PerFrameConstantBufferManager;
class FD3D12ResourceAllocator;
class FRenderer;

class FD3D12Manager : public EA::StdC::Singleton<FD3D12Manager>, public ID3D12RendererStateCallbackInterface
{
public:

	FD3D12Manager();
	FD3D12Manager(FD3D12Manager&&);
	FD3D12Manager& operator=(FD3D12Manager&&);
	~FD3D12Manager();
	void Init(FRenderer* const InRenderer);
	virtual void OnPreStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnPostEndFrame(FD3D12CommandContext& InCommandContext);


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
	eastl::unique_ptr<FD3D12PerFrameConstantBufferManager> D3D12PerFrameConstantBufferManager;
	eastl::unique_ptr<FD3D12ResourceAllocator> D3D12ResourceAllocator;

	eastl::vector<ID3D12RendererStateCallbackInterface*> TickedManagerList;
};

