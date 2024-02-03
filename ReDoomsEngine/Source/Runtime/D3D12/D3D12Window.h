#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12ManagerInterface.h"

class FD3D12Window : public EA::StdC::Singleton<FD3D12Window>, public ID3D12ManagerInterface
{
public:

	FD3D12Window(const long InWidth, const long InHeight, const wchar_t* const InWindowTitle);
	void Init();

	virtual void OnStartFrame();
	virtual void OnEndFrame();

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

