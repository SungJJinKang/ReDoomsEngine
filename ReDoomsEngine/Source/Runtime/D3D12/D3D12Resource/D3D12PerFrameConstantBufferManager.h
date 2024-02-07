#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12RendererStateCallbackInterface.h"

class FD3D12ConstantBufferResource;

struct FD3D12ConstantBufferBlock
{
	eastl::shared_ptr<FD3D12ConstantBufferResource> Resource{};
	D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress;

	uint8_t* MappedAddress;

	uint64_t SizeInBytes;
	uint64_t OffsetFromBase;
};

struct FD3D12PerFrameConstantBuffer
{
	eastl::shared_ptr<FD3D12ConstantBufferResource> ConstantBufferResource{};
	uint64_t CurrentOffset = 0;

	void Reset();
};

class FD3D12PerFrameConstantBufferManager : public EA::StdC::Singleton<FD3D12PerFrameConstantBufferManager>, public ID3D12RendererStateCallbackInterface
{
public:

	void Init();
	FD3D12ConstantBufferBlock Allocate(const uint64_t InSize);

	virtual void OnPreStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

private:

	eastl::array<FD3D12PerFrameConstantBuffer, GNumBackBufferCount> ConstantBufferRingBufferList;
};

