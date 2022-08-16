#pragma once
#include <Windows.h>

class GameApp;
class WindowAdapter
{
};


class WindowsWindow: public WindowAdapter
{
    GameApp* appInstance = nullptr;
    RECT windowRect = RECT{0, 0, 1080, 720};

    struct WindowSize
    {
        LONG width;
        LONG height;
    };

public:
    static HWND windowInst;

    int Run();
    void Initialize(GameApp* app, HINSTANCE hInstance, int nCmdShow);

    WindowSize GetWindowSize()
    {
        return WindowSize{ windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };
    }
};
