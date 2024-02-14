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

	bool bQuit = false;

	FCPUTimer TickTimer{"FrameTime"};

	while (!bQuit)
	{
		++GCurrentFrameIndex;
		CPUTimerBeginFrame();

		TickTimer.UpdateElapsedTicks();
		GTimeDelta = TickTimer.GetElapsedSeconds();

		{
			SCOPED_CPU_TIMER(Tick)

			// Poll and handle messages (inputs, window resize, etc.)
			// See the WndProc() function below for our to dispatch events to the Win32 backend.
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					bQuit = true;
			}
			if (bQuit)
				break;

			TestRenderer.Tick();
		}

		CPUTimerEndFrame();
	}

	TestRenderer.Destroy();

	return 0;
}