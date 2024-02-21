#include "D3D12Window.h"
#include "WindowsApplication.h"
#include "Renderer/Renderer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"

FD3D12Window::FD3D12Window(const long InWidth, const long InHeight, const wchar_t* const InWindowTitle, FRenderer* const InRenderer)
    :
    TargetRenderer(InRenderer),
    WindowHandle(),
    Width(InWidth),
    Height(InHeight),
    WindowTitle(InWindowTitle)
{
}

void FD3D12Window::Init()
{ 
    // Initialize the window class.
    WNDCLASSEX WindowClass;
    MEM_ZERO(WindowClass);
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = FD3D12Window::WindowProc;
    WindowClass.hInstance = FWindowsApplication::HInstance;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName = EA_WCHAR("ReDoomsEngine");
    RegisterClassExW(&WindowClass);

    RECT WindowRect = { 0, 0, Width, Height };
    AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    WindowHandle = CreateWindowW(
        WindowClass.lpszClassName,
        WindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowRect.right - WindowRect.left,
        WindowRect.bottom - WindowRect.top,
        nullptr,        // We have no parent window.
        nullptr,        // We aren't using menus.
        FWindowsApplication::HInstance,
        TargetRenderer);
    
    RD_CLOG(!WindowHandle, ELogVerbosity::Fatal, EA_WCHAR("\"CreateWindow\" fail (Error Code : %lu)"), GetLastError());

    ShowWindow(WindowHandle, FWindowsApplication::NumCmdShow);
}

void FD3D12Window::OnStartFrame(FD3D12CommandContext& InCommandContext)
{

}

void FD3D12Window::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}

LRESULT FD3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SIZE:
        // https://github.com/ocornut/imgui/issues/6764#issuecomment-1698835216
        // I was going to suggest that nowadays I wouldn't recommend performing the swap chain resize directly inside the WM_SIZE handler, 
        // because that tends to stress GPU drivers unnecessarily as the resizing operation is modal/blocking anyway, so the result is not seen immediately.
		if (GetD3D12Device() && wParam != SIZE_MINIMIZED)
		{
            FD3D12Manager::GetInstance()->GetSwapchain()->QueueResize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

