#include "CommonInclude.h"

#include "Commandline.h"
#include "WindowsApplication.h"
#include "D3D12Window.h"
#include "D3D12TestRenderer.h"
#include <windows.h>

int main(int argc, char** argv)
{
	{
		InitMemoryCore();
	}

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		return 1;
	}

	{
		char ConsoleTitle[500];  // to hold ConsoleTitle

		// get ConsoleTitle of console window
		GetConsoleTitleA(ConsoleTitle, 500);

		// get HWND of console, based on its ConsoleTitle
		HWND hwndConsole = FindWindowA(NULL, ConsoleTitle);
		FWindowsApplication::HInstance = (HINSTANCE)GetWindowLongPtr(hwndConsole, GWLP_HINSTANCE);
		FWindowsApplication::NumCmdShow = SW_SHOW;
	}

	FCommandline::InitCommandLine();

	D3D12TestRenderer TestRenderer{};
	TestRenderer.Init();

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	TestRenderer.Destroy();

	return 0;
}