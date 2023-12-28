#pragma once
#include "CommonInclude.h"

class FCommandline
{
public:
	static void InitCommandLine();
	static const eastl::wstring& CommandLine();

private:
	static eastl::wstring CachedCommandLine;
	static bool bInit;
};

