#include "D3D12Fence.h"

#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12CommandQueue.h"

FD3D12Fence::FD3D12Fence(const bool bInit)
	: D3DFence(), LastSignaledValue(0), bInterruptAwaited(false)
{
	if (bInit)
	{
		InitIfRequired();
	}
}

FD3D12Fence::~FD3D12Fence()
{
	Release();
}

void FD3D12Fence::InitIfRequired()
{
	if (!IsInit())
	{
		CreateD3DFence();
	}
}

void FD3D12Fence::InitIfRequired(const wchar_t* const InDebugName)
{
	if (!IsInit())
	{
		CreateD3DFence(InDebugName);
	}
}

void FD3D12Fence::Release()
{
	if (IsInit())
	{
		// @todo implement fence pool and free the fence to it
		D3DFence = nullptr;
	}
}

void FD3D12Fence::CreateD3DFence()
{
	EA_ASSERT(!IsInit());

	VERIFYD3D12RESULT(GetD3D12Device()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&D3DFence)
	));
}

void FD3D12Fence::CreateD3DFence(const wchar_t* const InDebugName)
{
	EA_ASSERT(!IsInit());

	CreateD3DFence();
	SetDebugNameToFence(InDebugName);
}

#if D3D_NAME_OBJECT
void FD3D12Fence::SetDebugNameToFence(const wchar_t* const InDebugName)
{
	EA_ASSERT(IsInit());

	D3DFence->SetName(InDebugName);
}
#endif

uint64_t FD3D12Fence::Signal(FD3D12CommandQueue* const InCommandQueue, const bool bWaitInstantly)
{
	EA_ASSERT(IsInit());

	const uint64_t SignaledValue = ++LastSignaledValue;
	VERIFYD3D12RESULT(InCommandQueue->GetD3DCommandQueue()->Signal(GetD3DFence(), SignaledValue));

	if (bWaitInstantly)
	{
		CPUWaitOnSignal(SignaledValue);
	}

	return SignaledValue;
}

void FD3D12Fence::CPUWaitOnSignal(const uint64_t SignaledValue)
{
	EA_ASSERT(IsInit());
	do {} while (!IsCompleteSignal(SignaledValue));
}

void FD3D12Fence::CPUWaitOnLastSignal()
{
	EA_ASSERT(IsInit());
	if (LastSignaledValue > 0)
	{
		do {} while (!IsCompleteLastSignal());
	}
}

void FD3D12Fence::GPUWaitOnSignal(FD3D12CommandQueue* const InCommandQueue, const uint64_t SignaledValue)
{
	EA_ASSERT(IsInit());
	InCommandQueue->GetD3DCommandQueue()->Wait(GetD3DFence(), SignaledValue);
}

void FD3D12Fence::GPUWaitOnLastSignal(FD3D12CommandQueue* const InCommandQueue)
{
	EA_ASSERT(IsInit());
	if (LastSignaledValue > 0)
	{
		InCommandQueue->GetD3DCommandQueue()->Wait(GetD3DFence(), LastSignaledValue);
	}
}

bool FD3D12Fence::IsCompleteSignal(const uint64_t SignaledValue)
{
	EA_ASSERT(IsInit());
	EA_ASSERT(SignaledValue > 0);
	return GetD3DFence()->GetCompletedValue() >= (SignaledValue);
}

bool FD3D12Fence::IsCompleteLastSignal()
{
	EA_ASSERT(IsInit());

	if (LastSignaledValue > 0)
	{
		return IsCompleteSignal(LastSignaledValue);
	}
	else
	{
		return true;
	}
}
