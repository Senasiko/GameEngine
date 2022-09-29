#pragma once
#include "GameController.h"
#include "WindowAdapter.h"
#include "pch.h"

class GameApp
{

    std::unique_ptr<WindowsWindow> winInst = nullptr;
    // std::unique_ptr<Renderer> renderer = nullptr;

public:
    void OnUpdate();
    void OnRender();
    void Initialize(HINSTANCE hInstance, int nCmdShow);

    std::unique_ptr<GameController> controller = nullptr;
    int Run();
};
