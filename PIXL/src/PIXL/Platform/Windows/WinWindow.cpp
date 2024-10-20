#include "pixlpch.h"
#include "WinWindow.h"


namespace PIXL {

    WinWindow::WinWindow(WinArgs winargs, const WindowProps& props) : hwnd(nullptr)
    {
        Init(winargs, props);
    }

    void WinWindow::Init(WinArgs winargs, const WindowProps& props)
	{
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = winargs.hins;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"PIXLWindow";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        RegisterClassExW(&wc);

        hwnd = CreateWindowExW(
            0,
            wc.lpszClassName,
            props.title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            props.width,
            props.height,
            NULL,
            NULL,
            winargs.hins,
            NULL
        );

        ShowWindow(hwnd, winargs.showcmd);
	}

    LRESULT CALLBACK PIXL::WinWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }

}