#include "D3D12Fence.h"

#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12CommandQueue.h"

void FD3D12Fence::CreateD3DFence()
{
	CreateD3DFence(EA_WCHAR("Unnamed"));
}

void FD3D12Fence::CreateD3DFence(const eastl::wstring& InDebugName)
{
	VERIFYD3D12RESULT(GetD3D12Device()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&D3DFence)
	));
	SetDebugNameToFence(InDebugName);
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
