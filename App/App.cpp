#include "pch.h"
#include "GameApp.h"
#include "App.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE , LPSTR , int nCmdShow)
{
    GameApp app;
    app.Initialize(hInstance, nCmdShow);
    return app.Run();
}   