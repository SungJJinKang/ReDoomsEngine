#pragma once

#include "CommonInclude.h"

class FWindow
{
public:

	static void Init(HINSTANCE HInstance, const int nCmdShow, const long Width, const long Height, const wchar_t* const WindowTitle);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static HWND GetWindowHandle()
	{
		return WindowHandle;
	}

private:

	static HWND WindowHandle;
};

