#include "Log.h"

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

void redooms::log::LogInternal(const ELogVerbosity LogVerbosity, const char* const FilePath, const uint32_t CodeLine, const wchar_t* const Format, ...)
{
	va_list Args;
	va_start(Args, Format);

	wchar_t StringBuffer[1024];
	EA::StdC::Vsprintf(StringBuffer, Format, Args);
	EA::StdC::Printf(EA_WCHAR("%s : %s (FilePath : %s, CodeLine : %d)"), LogVerbosityToString(LogVerbosity), StringBuffer, FilePath, CodeLine);
	va_end(Args);


	if (LogVerbosity == ELogVerbosity::Error)
	{
		EA_DEBUG_BREAK();
	}
	else if (LogVerbosity == ELogVerbosity::Fatal)
	{
		EA_CRASH();
	}
}
