#pragma once
#include "CommonInclude.h"

#include <BaseTsd.h>

class FWindowsApplication
{
public:
	static HINSTANCE HInstance;
	static int NumCmdShow;

	static const eastl::wstring& const GetExecutablePath();
	static const eastl::wstring& const GetExecutableDirectory();

private:

};
