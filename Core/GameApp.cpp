#include "GameApp.h"
#include "pix3.h"
#include <filesystem>
#include <memory>
#include <shlobj.h>

#include "Mesh.h"
#if defined(DEBUG) | defined(_DEBUG)
std::wstring GetLatestWinPixGpuCapturerPath()
{
    LPWSTR programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

    std::filesystem::path pixInstallationPath = programFilesPath;
    pixInstallationPath /= "Microsoft PIX";

    std::wstring newestVersionFound;

    for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
    {
        if (directory_entry.is_directory())
        {
            if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
            {
                newestVersionFound = directory_entry.path().filename().c_str();
            }
        }
    }

    if (newestVersionFound.empty())
    {
        // TODO: Error, no PIX installation found
    }

    return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}
#endif

void GameApp::Initialize(HINSTANCE hInstance, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)

    if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
    {
        LoadLibrary(GetLatestWinPixGpuCapturerPath().c_str());
    }
#endif
    winInst = std::make_unique<WindowsWindow>();
    controller = std::make_unique<GameController>();
    winInst->Initialize(this, hInstance, nCmdShow);
    auto size = winInst->GetWindowSize();
    renderer->Initialize(size.width, size.height);
}

int GameApp::Run()
{
    auto a = Mesh();
    renderer->scene->AddObject(std::make_shared<Mesh>());
    renderer->scene->AddLight(std::make_shared<DirectLight>());
    return winInst->Run();
}

void GameApp::OnUpdate()
{
    renderer->Update();
}


void GameApp::OnRender()
{
}

