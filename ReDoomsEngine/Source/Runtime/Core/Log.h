#pragma once

#include "Macros.h"

enum class ELogVerbosity
{
	Log,
	Warning,
	Error,
	Fatal
};

namespace redooms::log
{
	const wchar_t* LogVerbosityToString(const ELogVerbosity LogVerbosity);
	void LogInternal(const ELogVerbosity LogVerbosity, const wchar_t* const FilePath, const unsigned int CodeLine, const wchar_t* const Format, ...);
}

#if RD_DEBUG

#define RD_LOG(LogVerbosity, Format, ...) \
{ \
	redooms::log::LogInternal(LogVerbosity, EA_WCHAR(__FILE__), __LINE__, Format, ##__VA_ARGS__); \
}

#define RD_CLOG(Condition, LogVerbosity, Format, ...) \
{ \
	if(Condition) \
	{ \
		redooms::log::LogInternal(LogVerbosity, EA_WCHAR(__FILE__), __LINE__, Format, ##__VA_ARGS__); \
	} \
}

#else

#define RD_LOG(LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { redooms::log::LogInternal(LogVerbosity, EA_WCHAR(__FILE__), __LINE__, Format, ##__VA_ARGS__); } }
#define RD_CLOG(Condition, LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { if(Condition) redooms::log::LogInternal(LogVerbosity, EA_WCHAR(__FILE__), __LINE__, Format, ##__VA_ARGS__); } }

#endif
