#include "WindowsApplication.h"

#include "EAStdC/EAProcess.h"

HINSTANCE FWindowsApplication::HInstance{};
int FWindowsApplication::NumCmdShow{ -1 };

const eastl::wstring& FWindowsApplication::GetExecutablePath()
{
	static eastl::wstring ExecutablePath{};
	if (ExecutablePath.empty())
	{
		wchar_t TempPath[MAX_PATH + 1];
		EA::StdC::GetCurrentProcessPath(TempPath, MAX_PATH + 1);
		ExecutablePath.assign(TempPath);
		EA_ASSERT(!(ExecutablePath.empty()));
	}
	return ExecutablePath;
}

const eastl::wstring& FWindowsApplication::GetExecutableDirectory()
{
	static eastl::wstring ExecutableDirectory{};
	if (ExecutableDirectory.empty())
	{
		wchar_t TempPath[MAX_PATH + 1];
		EA::StdC::GetCurrentProcessDirectory(TempPath, MAX_PATH + 1);
		ExecutableDirectory.assign(TempPath);
		EA_ASSERT(!(ExecutableDirectory.empty()));
	}
	return ExecutableDirectory;
}
