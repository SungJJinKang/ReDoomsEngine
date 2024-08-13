#include "Log.h"
#include "CommonInclude.h"

const wchar_t* redooms::log::LogVerbosityToString(const ELogVerbosity LogVerbosity)
{
	switch (LogVerbosity)
	{
	case ELogVerbosity::Log:
			return EA_WCHAR("Log");
	case ELogVerbosity::Warning:
		return EA_WCHAR("Warning");
	case ELogVerbosity::Error:
		return EA_WCHAR("Error");
	case ELogVerbosity::Fatal:
		return EA_WCHAR("Fatal");
	default:
		RD_ASSUME(0);
	}
}

#define LOG_STRING_BUFFER_LENGTH 10240

void redooms::log::LogInternal(const ELogVerbosity LogVerbosity, const wchar_t* const FilePath, const unsigned int CodeLine, const wchar_t* const Format, ...)
{
	wchar_t Buffer[LOG_STRING_BUFFER_LENGTH];
	va_list Args;
	va_start(Args, Format);
	const int CharCount = EA::StdC::Vsnprintf(Buffer, LOG_STRING_BUFFER_LENGTH, Format, Args);
	EA_ASSERT(CharCount + 1<= LOG_STRING_BUFFER_LENGTH);
	Buffer[CharCount] = '\n';
	Buffer[CharCount + 1] = 0;

	va_end(Args);

	if (IsDebuggerPresent())
	{
		OutputDebugStringW(Buffer);
	}
	else
	{
		EA::StdC::Printf(Buffer);
	}

	if (LogVerbosity == ELogVerbosity::Error)
	{
		EA_DEBUG_BREAK();
	}
	else if (LogVerbosity == ELogVerbosity::Fatal)
	{
		EA_CRASH();
	}
}
