#include "D3D12Fence.h"

#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12CommandQueue.h"

FD3D12Fence::FD3D12Fence()
{
}

FD3D12Fence::FD3D12Fence(FD3D12Device* const InDevice)
{
	CreateD3DFence(InDevice);
}

FD3D12Fence::FD3D12Fence(FD3D12Device* const InDevice, const eastl::wstring& InDebugName)
{
	CreateD3DFence(InDevice);
	SetDebugNameToFence(InDebugName);
}

void FD3D12Fence::CreateD3DFence(FD3D12Device* const InDevice)
{
	VERIFYD3D12RESULT(InDevice->GetD3D12Device()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&D3DFence)
	));
}

void FD3D12Fence::SetDebugNameToFence(const eastl::wstring& InDebugName)
{
	D3DFence->SetName(InDebugName.c_str());
}

uint64_t FD3D12Fence::Signal(FD3D12CommandQueue* const InCommandQueue)
{
	const uint64_t SignaledValue = LastSignaledValue;
	VERIFYD3D12RESULT(InCommandQueue->GetD3DCommandQueue()->Signal(GetD3DFence(), SignaledValue));
	++LastSignaledValue;

	return SignaledValue;
}
