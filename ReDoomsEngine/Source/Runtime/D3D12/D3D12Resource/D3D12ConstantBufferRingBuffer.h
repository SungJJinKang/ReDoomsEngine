#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "Common/RendererStateCallbackInterface.h"

class FD3D12ConstantBufferResource;

struct FD3D12ConstantBufferBlock
{
	FD3D12ConstantBufferResource* OwnerResource{};
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

class FD3D12ConstantBufferRingBuffer : public EA::StdC::Singleton<FD3D12ConstantBufferRingBuffer>, public IRendererStateCallbackInterface
{
public:

	void Init();
	FD3D12ConstantBufferBlock Allocate(uint64_t InSize);

	virtual void OnPreStartFrame();

	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext) {}
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext) {}

	static constexpr uint64_t AllocatableRendererState = ERendererState::OnStartFrame | ERendererState::Draw;

private:

	FD3D12PerFrameConstantBuffer ConstantBufferRingBuffer;
	bool bAnyAllocedThisFrame{ false };
	eastl::array<uint64_t, GNumBackBufferCount> OffsetFromBaseWhenStartFrameList{ 0 };

};

