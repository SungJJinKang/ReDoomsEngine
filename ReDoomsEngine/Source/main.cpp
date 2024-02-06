#include "CommonInclude.h"

#include "Commandline.h"
#include "WindowsApplication.h"
#include "D3D12Window.h"
#include "D3D12TestRenderer.h"

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int32_t nCmdShow)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		return 1;
	}

	FWindowsApplication::HInstance = hInstance;
	FWindowsApplication::NumCmdShow = nCmdShow;

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
