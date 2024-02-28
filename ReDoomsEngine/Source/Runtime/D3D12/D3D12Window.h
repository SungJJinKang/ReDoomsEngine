#pragma once
#include "CommonInclude.h"

#include "EASTL/functional.h"

#include "D3D12Include.h"
#include "Common/RendererStateCallbackInterface.h"

class FRenderer;
class FD3D12Window : public EA::StdC::Singleton<FD3D12Window>, public IRendererStateCallbackInterface
{
public:

	FD3D12Window(const long InWidth, const long InHeight, const wchar_t* const InWindowTitle, FRenderer* const InRenderer);
	void Init();

	virtual void OnStartFrame(FD3D12CommandContext& InCommandContext);
	virtual void OnEndFrame(FD3D12CommandContext& InCommandContext);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	inline HWND GetWindowHandle() const
	{
		EA_ASSERT(WindowHandle);
		return WindowHandle;
	}

	inline static bool WKeyPressed = false;
	inline static bool AKeyPressed = false;
	inline static bool SKeyPressed = false;
	inline static bool DKeyPressed = false;
	inline static bool LeftArrowKeyPressed = false;
	inline static bool RIghtArrowKeyPressed = false;
	inline static bool UpArrowKeyPressed = false;
	inline static bool DownArrowKeyPressed = false;

private:

	FRenderer* const TargetRenderer;
	HWND WindowHandle;

	int32_t Width;
	int32_t Height;
	const wchar_t* WindowTitle;
};

