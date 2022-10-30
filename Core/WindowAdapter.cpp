#include <iostream>

#include "WindowAdapter.h"
#include "GameApp.h"

HWND WindowsWindow::windowInst;

LRESULT CALLBACK Listener(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GameApp* app = reinterpret_cast<GameApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (message)
    {
    case WM_CREATE:
        {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;
    case WM_KEYDOWN:
        if (app)
        {
            DebugLog("down")
            app->controller->OnKeyDown(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_KEYUP:
        if (app)
        {
            app->controller->OnKeyUp(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_PAINT:
        if (app)
        {
            app->OnUpdate();
            app->OnRender();
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}        

void WindowsWindow::Initialize(GameApp* app, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.lpfnWndProc = Listener;
    wcex.hInstance      = hInstance;
    wcex.lpszClassName = L"WindowsWindow";
    wcex.lpszMenuName = nullptr;
    wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassExW(&wcex);
    windowInst = CreateWindow(wcex.lpszClassName, L"Game", WS_OVERLAPPEDWINDOW, 100, 100, GetWindowSize().width, GetWindowSize().height, nullptr, nullptr, hInstance, app);

    auto windowR = windowRect;
    AdjustWindowRect(&windowR, WS_OVERLAPPEDWINDOW, FALSE);
    ShowWindow(windowInst, nCmdShow);
    
}


int WindowsWindow::Run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return static_cast<int>(msg.wParam);
}
