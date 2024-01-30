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
		EA_ASSUME(0);
	}
}

#define LOG_STRING_BUFFER_LENGTH 2048

void redooms::log::LogInternal(const ELogVerbosity LogVerbosity, const wchar_t* const FilePath, const unsigned int CodeLine, const wchar_t* const Format, ...)
{
	if (IsDebuggerPresent())
	{
		va_list Args;
		va_start(Args, Format);

		
		wchar_t StringBuffer[LOG_STRING_BUFFER_LENGTH];
		const int LogLength = EA::StdC::Vsnprintf(StringBuffer, LOG_STRING_BUFFER_LENGTH, Format, Args);
		EA_ASSERT(LogLength < LOG_STRING_BUFFER_LENGTH);
		EA::StdC::Snprintf(StringBuffer + LogLength, LOG_STRING_BUFFER_LENGTH - LogLength, EA_WCHAR(" (LogVerbosity : %s)\n"), LogVerbosityToString(LogVerbosity));
		OutputDebugString(StringBuffer);
		va_end(Args);
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
