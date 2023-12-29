#pragma once

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
	void LogInternal(const ELogVerbosity LogVerbosity, const char* const FilePath, const unsigned int CodeLine, const wchar_t* const Format, ...);
}

#if defined(_DEBUG)

#define RD_LOG(LogVerbosity, Format, ...) \
{ \
	redooms::log::LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); \
}

#define RD_CLOG(Condition, LogVerbosity, Format, ...) \
{ \
	if(Condition) \
	{ \
		redooms::log::LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); \
	} \
}

#else

#define RD_LOG(LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { redooms::log::LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); } }
#define RD_CLOG(Condition, LogVerbosity, Format, ...) { if(LogVerbosity == ELogVerbosity::Fatal) { if(Condition) redooms::log::LogInternal(LogVerbosity, __FILE__, __LINE__, Format, ##__VA_ARGS__); } }

#endif
