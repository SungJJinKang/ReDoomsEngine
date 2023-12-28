#pragma once
#include "CommonInclude.h"

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
	void LogInternal(const ELogVerbosity LogVerbosity, const char* const FilePath, const uint32_t CodeLine, const wchar_t* const Format, ...);
}

#if defined(_DEBUG)

#define RD_LOG(LogVerbosity, Format, ...) \
{ \
	LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); \
}

#define RD_CLOG(Condition, LogVerbosity, Format, ...) \
{ \
	if(Condition) \
	{ \
		LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); \
	} \
}

#else

#define RD_LOG(LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); } }
#define RD_CLOG(Condition, LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { if(Condition) LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); } }

#endif
