#include "Window.h"

HWND FWindow::WindowHandle{};

void FWindow::Init(HINSTANCE HInstance, const int nCmdShow, const long Width, const long Height, const wchar_t* const WindowTitle)
{ // Initialize the window class.
    WNDCLASSEX WindowClass = { 0 };
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = FWindow::WindowProc;
    WindowClass.hInstance = HInstance;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName = EA_WCHAR("ReDoomsEngine");
    RegisterClassEx(&WindowClass);

    RECT WindowRect = { 0, 0, Width, Height };
    AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    WindowHandle = CreateWindow(
        WindowClass.lpszClassName,
        WindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowRect.right - WindowRect.left,
        WindowRect.bottom - WindowRect.top,
        nullptr,        // We have no parent window.
        nullptr,        // We aren't using menus.
        HInstance,
        NULL);
}

LRESULT FWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return LRESULT();
}
