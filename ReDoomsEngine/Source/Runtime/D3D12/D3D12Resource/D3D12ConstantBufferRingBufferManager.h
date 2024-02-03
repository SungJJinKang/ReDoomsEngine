#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12ManagerInterface.h"

class FD3D12ConstantBufferResource;

struct FD3D12ConstantBufferBlock
{
	eastl::shared_ptr<FD3D12ConstantBufferResource> Resource{};
	D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress;

	uint8_t* MappedAddress;

	uint64_t SizeInBytes;
	uint64_t OffsetFromBase;
};

struct FD3D12ConstantBufferRingBuffer
{
	eastl::shared_ptr<FD3D12ConstantBufferResource> ConstantBufferResource{};
	uint64_t CurrentOffset = 0;

	void Reset();
};

class FD3D12ConstantBufferRingBufferManager : public EA::StdC::Singleton<FD3D12ConstantBufferRingBufferManager>, public ID3D12ManagerInterface
{
public:

	void Init();
	FD3D12ConstantBufferBlock Allocate(const uint64_t InSize);

	virtual void OnPreStartFrame();
	virtual void OnStartFrame();
	virtual void OnEndFrame();

private:

	eastl::array<FD3D12ConstantBufferRingBuffer, GNumBackBufferCount> ConstantBufferRingBufferList;
};

