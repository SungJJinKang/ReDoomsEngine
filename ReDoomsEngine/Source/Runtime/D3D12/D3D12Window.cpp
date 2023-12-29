#include "D3D12Window.h"

FD3D12Window::FD3D12Window(const long InWidth, const long InHeight, const wchar_t* const InWindowTitle)
    :
    WindowHandle(),
    Width(InWidth),
    Height(InHeight),
    WindowTitle(InWindowTitle)
{
}

void FD3D12Window::Init()
{ 
    // Initialize the window class.
    WNDCLASSEX WindowClass = { 0 };
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
        NULL);
    
    RD_CLOG(!WindowHandle, ELogVerbosity::Fatal, EA_WCHAR("\"CreateWindow\" fail (Error Code : %lu)"), GetLastError());

    ShowWindow(WindowHandle, FWindowsApplication::NumCmdShow);
}

LRESULT FD3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Save the DXSample* passed in to CreateWindow.
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;

    case WM_KEYDOWN:
        
        return 0;

    case WM_KEYUP:
        
        return 0;

    case WM_PAINT:
        
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, message, wParam, lParam);
}

