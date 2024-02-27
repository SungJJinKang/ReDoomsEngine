#pragma once
#include "EAStdC/EAString.h"
#include "EAStdC/EASprintf.h"
#include "EASTL/string.h"

// Don't store returned address in char(wchar) pointer. The address will points to garbage data 
#define WCHAR_TO_UTF8(Str) EA::StdC::ConvertString<eastl::wstring_view, eastl::string>(Str).c_str()
#define UTF8_TO_WCHAR(Str) EA::StdC::ConvertString<eastl::u8string_view, eastl::wstring>(Str).c_str()
#define ANSI_TO_WCHAR(Str) UTF8_TO_WCHAR(Str)

template <typename StrType, typename ReturnType = eastl::basic_string<typename eastl::remove_const<typename eastl::remove_pointer<StrType>::type>::type>>
ReturnType PrintfHelper(StrType pFormat, ...)
{
	ReturnType String{};

	va_list arguments;
	va_start(arguments, pFormat);

	const int Result = EA::StdC::StringVcprintf(String, pFormat, arguments);
	EA_ASSERT(Result != -1);

	va_end(arguments);

	return String;
}

#define FORMATTED_CHAR(Format, ...) PrintfHelper(Format, __VA_ARGS__).c_str()
#define FORMATTED_WCHAR(Format, ...) PrintfHelper( EA_WCHAR(Format) , __VA_ARGS__).c_str()

#define RD_CONCAT(a, b) RD_CONCAT_INNER(a, b)
#define RD_CONCAT_INNER(a, b) a ## b

#define RD_UNIQUE_NAME(base) RD_CONCAT(base, __COUNTER__)