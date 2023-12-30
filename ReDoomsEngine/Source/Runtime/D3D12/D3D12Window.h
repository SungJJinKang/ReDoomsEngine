#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Window : public EA::StdC::Singleton<FD3D12Window>
{
public:

	FD3D12Window(const long InWidth, const long InHeight, const wchar_t* const InWindowTitle);
	void Init();

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	inline HWND GetWindowHandle() const
	{
		EA_ASSERT(WindowHandle);
		return WindowHandle;
	}

private:

	HWND WindowHandle;

	int32_t Width;
	int32_t Height;
	const wchar_t* WindowTitle;
};

