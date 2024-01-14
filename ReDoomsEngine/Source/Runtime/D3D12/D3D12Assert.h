#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

namespace d3d12
{
	namespace assert
	{
		eastl::wstring GetD3D12ErrorString(HRESULT Result);
		void D3D12CallFail(HRESULT Result, const wchar_t* const Statement, const wchar_t* const FilePath, const uint32_t CodeLine);
		void D3D12CallFail(HRESULT Result, const wchar_t* const Statement, const wchar_t* const FilePath, const uint32_t CodeLine, const ComPtr<ID3DBlob>& ErrorBlob);
	}
}
#define VERIFYD3D12RESULT(Statement) {HRESULT Result = Statement; if (FAILED(Result)) { d3d12::assert::D3D12CallFail(Result, EA_WCHAR(#Statement), EA_WCHAR(__FILE__), __LINE__); }}
#define VERIFYD3D12RESULT_ERRORBLOB(Statement, ErrorBlob) {HRESULT Result = Statement; if (FAILED(Result)) { d3d12::assert::D3D12CallFail(Result, EA_WCHAR(#Statement), EA_WCHAR(__FILE__), __LINE__, ErrorBlob); }}