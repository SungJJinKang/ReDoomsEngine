#pragma once
#include "CommonInclude.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class FWindowsApplication
{
public:
	static HINSTANCE HInstance;
	static int NumCmdShow;

	static const wchar_t* const GetExecutablePath();

private:

	static bool bIsExecutablePathSet;
	static wchar_t ExecutablePath[MAX_PATH + 1];
};

