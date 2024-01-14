#include "D3D12Assert.h"

#define D3DERR(x) case x: ErrorCodeText = TEXT(#x); break;

static EA::Thread::Futex VerifyD3D12ResultFutex{};

eastl::wstring d3d12::assert::GetD3D12ErrorString(HRESULT Result)
{
	eastl::wstring ErrorCodeText;

	switch (Result)
	{
		D3DERR(S_OK);
		D3DERR(D3D11_ERROR_FILE_NOT_FOUND)
		D3DERR(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
		D3DERR(E_FAIL)
		D3DERR(E_INVALIDARG)
		D3DERR(E_OUTOFMEMORY)
		D3DERR(DXGI_ERROR_INVALID_CALL)
		D3DERR(DXGI_ERROR_WAS_STILL_DRAWING)
		D3DERR(E_NOINTERFACE)
		D3DERR(DXGI_ERROR_DEVICE_REMOVED)
		D3DERR(DXGI_ERROR_UNSUPPORTED)
		D3DERR(DXGI_ERROR_NOT_CURRENT)
		D3DERR(DXGI_ERROR_MORE_DATA)
		D3DERR(DXGI_ERROR_MODE_CHANGE_IN_PROGRESS)
		D3DERR(DXGI_ERROR_ALREADY_EXISTS)
		D3DERR(DXGI_ERROR_SESSION_DISCONNECTED)
		D3DERR(DXGI_ERROR_ACCESS_DENIED)
		D3DERR(DXGI_ERROR_NON_COMPOSITED_UI)
		D3DERR(DXGI_ERROR_CACHE_FULL)
		D3DERR(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		D3DERR(DXGI_ERROR_CACHE_CORRUPT)
		D3DERR(DXGI_ERROR_WAIT_TIMEOUT)
		D3DERR(DXGI_ERROR_FRAME_STATISTICS_DISJOINT)
		D3DERR(DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION)
		D3DERR(DXGI_ERROR_REMOTE_OUTOFMEMORY)
		D3DERR(DXGI_ERROR_ACCESS_LOST)
	default:
		ErrorCodeText = EA::StdC::Printf(EA_WCHAR("%08X"), (int32_t)Result);
	}

	return ErrorCodeText;
}

void d3d12::assert::D3D12CallFail(HRESULT Result, const wchar_t* const Statement, const wchar_t* const FilePath, const uint32_t CodeLine)
{
	EA::Thread::AutoFutex AutoFutex{ VerifyD3D12ResultFutex };

	if (Result == E_OUTOFMEMORY)
	{

	}
	else if (Result == DXGI_ERROR_DEVICE_REMOVED || Result == DXGI_ERROR_DEVICE_HUNG || Result == DXGI_ERROR_DEVICE_RESET)
	{

	}
	redooms::log::LogInternal(ELogVerbosity::Fatal, FilePath, CodeLine, EA_WCHAR("VerifyD3D12Result Fail : %s (Error Code : %ld)"), Statement, GetD3D12ErrorString(Result).c_str());
}

void d3d12::assert::D3D12CallFail(HRESULT Result, const wchar_t* const Statement, const wchar_t* const FilePath, const uint32_t CodeLine, const ComPtr<ID3DBlob>& ErrorBlob)
{
	EA::Thread::AutoFutex AutoFutex{ VerifyD3D12ResultFutex };

	if (Result == E_OUTOFMEMORY)
	{

	}
	else if (Result == DXGI_ERROR_DEVICE_REMOVED || Result == DXGI_ERROR_DEVICE_HUNG || Result == DXGI_ERROR_DEVICE_RESET)
	{

	}

	eastl::string8 ErrorStr{ reinterpret_cast<const char8_t*>(ErrorBlob->GetBufferPointer()), ErrorBlob->GetBufferSize() };
	redooms::log::LogInternal(ELogVerbosity::Fatal, FilePath, CodeLine, EA_WCHAR("VerifyD3D12Result Fail : %s (Error Code : %ld)(Error String : %s)"), Statement, GetD3D12ErrorString(Result).c_str(), UTF8_TO_WCHAR(ErrorStr));
}
