#include "Commandline.h"

#include "processenv.h"

bool FCommandline::bInit{ false };
eastl::wstring FCommandline::CachedCommandLine{};

void FCommandline::InitCommandLine()
{
    CachedCommandLine = GetCommandLineW();
    bInit = true;
}

const eastl::wstring& FCommandline::CommandLine()
{
	EA_ASSERT(bInit);
	return CachedCommandLine;
}