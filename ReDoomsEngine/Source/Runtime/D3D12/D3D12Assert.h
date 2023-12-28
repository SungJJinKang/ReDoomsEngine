#pragma once
#include "CommonInclude.h"

namespace d3d12
{
	namespace assert
	{
		eastl::wstring GetD3D12ErrorString(HRESULT Result);
		void VerifyD3D12Result(HRESULT Result, const char* const Statement, const char* const FilePath, const uint32_t CodeLine);
	}
}
#define VERIFYD3D12RESULT(Statement) {HRESULT Result = Statement; if (FAILED(Result)) { d3d12::assert::VerifyD3D12Result(Result, #Statement, __FILE__, __LINE__); }}