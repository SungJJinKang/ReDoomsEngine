#pragma once
#include "CommonInclude.h"

namespace d3d12
{
	namespace assert
	{
		eastl::wstring GetD3D12ErrorString(HRESULT Result);
		void D3D12CallFail(HRESULT Result, const wchar_t* const Statement, const wchar_t* const FilePath, const uint32_t CodeLine);
	}
}
#define VERIFYD3D12RESULT(Statement) {HRESULT Result = Statement; if (FAILED(Result)) { d3d12::assert::D3D12CallFail(Result, EA_WCHAR(#Statement), EA_WCHAR(__FILE__), __LINE__); }}