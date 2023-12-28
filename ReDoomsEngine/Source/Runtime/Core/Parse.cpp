#include "Parse.h"

bool FParse::ParseValue(const eastl::wstring& Stream, const wchar_t* const Match, eastl::wstring& OutString)
{
	return ParseValue(Stream.c_str(), Match, OutString);
}

bool FParse::ParseValue(const wchar_t* const Stream, const wchar_t* const Match, eastl::wstring& OutString)
{
	EA_ASSERT(Stream);
	EA_ASSERT(Match);

	if (const wchar_t* const FoundStr = EA::StdC::Stristr(Stream, Match))
	{
		const wchar_t* const MatchedValue = FoundStr + EA::StdC::Strlen(Match);
		const wchar_t* End = MatchedValue;

		while (!EA::StdC::Isspace(*End)) {}

		OutString.assign(MatchedValue, End);
		return true;
	}
	else
	{
		return false;
	}
}
