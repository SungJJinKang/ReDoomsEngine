#pragma once
#include "EAStdC/EAString.h"
#include "EASTL/string.h"

// Don't store returned address in char(wchar) pointer. The address points to garbage data 
#define UTF8_TO_WCHAR(Str) EA::StdC::ConvertString<eastl::u8string_view, eastl::wstring>(Str).c_str()
#define ANSI_TO_WCHAR(Str) UTF8_TO_WCHAR(Str)

#define RD_CONCAT(a, b) RD_CONCAT_INNER(a, b)
#define RD_CONCAT_INNER(a, b) a ## b

#define RD_UNIQUE_NAME(base) RD_CONCAT(base, __COUNTER__)