#pragma once

#include "CommonInclude.h"

class FParse
{
public:
	static bool ParseValue(const eastl::wstring& Stream, const wchar_t* const Match, eastl::wstring& OutString);
	static bool ParseValue(const wchar_t* const Stream, const wchar_t* const Match, eastl::wstring& OutString);
};

