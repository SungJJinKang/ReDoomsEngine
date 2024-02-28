#include "CommonInclude.h"

#include "Commandline.h"
#include "WindowsApplication.h"
#include "D3D12Window.h"
#include "Renderer/D3D12TestRenderer.h"
#include <windows.h>

int main(int argc, char** argv)
{
	TLSThreadName = "MainThread";
	TLSThreadType = ThreadType::MainThread;

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
	{
		SCOPED_CPU_TIMER(Renderer_Init)
		SCOPED_MEMORY_TRACE(Renderer_Init)
		TestRenderer.Init();
	}

	bool bQuit = false;

	while (!bQuit)
	{
		CPUTimerBeginFrame();

		{
			FCPUTimer FrameTimeTimer{ "FrameTime", false};
			FrameTimeTimer.Start();

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

			FrameTimeTimer.End();
			GTimeDelta = FrameTimeTimer.GetElapsedSeconds();

			CPUTimerEndFrame();

			++GCurrentFrameIndex;
			ResetD3D12GlobalVariablesForNewFrame();
		}
	}

	{
		SCOPED_CPU_TIMER(Renderer_Destroy)
		SCOPED_MEMORY_TRACE(Renderer_Destroy)
		TestRenderer.Destroy();
	}

	DestroyTimerData();

	return 0;
}