#include "CommonInclude.h"

#include "Commandline.h"
#include "Window.h"
#include "RendererManager.h"

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int32_t nCmdShow)
{
	FCommandline::InitCommandLine();

	FWindow::Init(hInstance, nCmdShow, 1024, 720, EA_WCHAR("Test"));

	FRendererManager RendererManager;
	
}
