#include "CommonInclude.h"

#include "Commandline.h"
#include "WindowsApplication.h"
#include "D3D12Window.h"
#include "D3D12TestRenderer.h"

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int32_t nCmdShow)
{
	FWindowsApplication::HInstance = hInstance;
	FWindowsApplication::NumCmdShow = nCmdShow;

	FCommandline::InitCommandLine();

	D3D12TestRenderer TestRenderer{};
	TestRenderer.Init();

	bool bExit = false;
	while (!bExit)
	{
		TestRenderer.OnPreStartFrame();
		TestRenderer.OnStartFrame();
		bExit = !(TestRenderer.Draw());
		TestRenderer.OnEndFrame();
		TestRenderer.OnPostEndFrame();
	}
	TestRenderer.Destroy();
	
	return 0;
}
