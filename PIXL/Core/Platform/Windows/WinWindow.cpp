#include "pch.h"
#include "WinWindow.h"

namespace GameSystem {

    WinWindow::WinWindow(const WindowProps& props) : hwnd(nullptr)
    {
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = HINSTANCE();
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
            HINSTANCE(),
            NULL
        );

        ShowWindow(hwnd, SW_SHOW);
    }

    bool WinWindow::Update()
    {
        MSG msg = {};
        bool proceed = true;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                proceed = false;
        }
        return proceed;
    }

    LRESULT CALLBACK WinWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }

}