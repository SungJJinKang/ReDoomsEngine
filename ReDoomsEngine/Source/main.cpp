#include "CommonInclude.h"

#include "Commandline.h"
#include "WindowsApplication.h"
#include "D3D12Window.h"
#include "RendererManager.h"

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int32_t nCmdShow)
{
	FWindowsApplication::HInstance = hInstance;
	FWindowsApplication::NumCmdShow = nCmdShow;

	FCommandline::InitCommandLine();

	FRendererManager RendererManager{};
	RendererManager.Init();
	
	return 0;
}
