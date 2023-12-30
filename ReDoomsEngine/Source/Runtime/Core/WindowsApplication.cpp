#include "WindowsApplication.h"

HINSTANCE FWindowsApplication::HInstance{};
int FWindowsApplication::NumCmdShow{ -1 };
bool FWindowsApplication::bIsExecutablePathSet{ false };
wchar_t FWindowsApplication::ExecutablePath[MAX_PATH + 1]{};

const wchar_t* const FWindowsApplication::GetExecutablePath()
{
	if (!bIsExecutablePathSet)
	{
		GetModuleFileNameW(NULL, ExecutablePath, MAX_PATH + 1);
		bIsExecutablePathSet = true;
	}
	return ExecutablePath;
}
