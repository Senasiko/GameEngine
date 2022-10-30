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

void sphereGenerate(std::vector<Vertex>& verts, std::vector<UINT16>& triangles, float radius, int slices, int segments)
{
    verts.resize((segments+1)*slices+2);

    const float _pi = XM_PI;
    const float _2pi = XM_2PI;

    verts[0].Position = XMFLOAT3(0, radius , 0) ;
    for (int lat = 0; lat < slices; lat++)
    {
        float a1 = _pi * (float)(lat + 1) / (slices + 1);
        float sin1 = sinf(a1);
        float cos1 = cosf(a1);

        for (int lon = 0; lon <= segments; lon++)
        {
            float a2 = _2pi * (float)(lon == segments ? 0 : lon) / segments;
            float sin2 = sinf(a2);
            float cos2 = cosf(a2);

            verts[lon + lat * (segments + 1) + 1].Position = XMFLOAT3( sin1 * cos2* radius, cos1* radius, sin1 * sin2* radius) ;
        }
    }
    verts[verts.size() - 1].Position = XMFLOAT3(0, -radius, 0);
	
    for (int n = 0; n < verts.size(); n++)
    {
        XMStoreFloat3(&verts[n].Normal, XMVector3Normalize( XMVectorSet(verts[n].Position.x, verts[n].Position.y, verts[n].Position.z, 1)));
        verts[n].Color = XMFLOAT4(1.0f, 0.f, 0.f, 1.f);
    }

    int nbFaces = verts.size();
    int nbTriangles = nbFaces * 2;
    int nbIndexes = nbTriangles * 3;

    triangles.resize(nbIndexes);
    //int* triangles = new int[nbIndexes];
    
    int i = 0;
    for (int lon = 0; lon < segments; lon++)
    {
        triangles[i++] = lon + 2;
        triangles[i++] = lon + 1;
        triangles[i++] = 0;
    }

    //Middle
    for (int lat = 0; lat < slices - 1; lat++)
    {
        for (int lon = 0; lon < segments; lon++)
        {
            int current = lon + lat * (segments + 1) + 1;
            int next = current + segments + 1;

            triangles[i++] = current;
            triangles[i++] = current + 1;
            triangles[i++] = next + 1;

            triangles[i++] = current;
            triangles[i++] = next + 1;
            triangles[i++] = next;
        }
    }

    //Bottom Cap
    for (int lon = 0; lon < segments; lon++)
    {
        triangles[i++] = verts.size() - 1;
        triangles[i++] = verts.size() - (lon + 2) - 1;
        triangles[i++] = verts.size() - (lon + 1) - 1;
    }
}

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
    auto cube = std::make_shared<Mesh>();
    std::array<Vertex, 8> vertices =
    {
        Vertex({XMFLOAT3(-4.0f, -4.0f, -4.0f), XMFLOAT4(Colors::White), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(-4.0f, 4.0f, -4.0f), XMFLOAT4(Colors::Black), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(4.0f, 4.0f, -4.0f), XMFLOAT4(Colors::Red), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(4.0f, -4.0f, -4.0f), XMFLOAT4(Colors::Green), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(-4.0f, -4.0f, 4.0f), XMFLOAT4(Colors::Blue), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(-4.0f, 4.0f, 4.0f), XMFLOAT4(Colors::Yellow), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(4.0f, 4.0f, 4.0f), XMFLOAT4(Colors::Cyan), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(4.0f, -4.0f, 4.0f), XMFLOAT4(Colors::Magenta), XMFLOAT2(1, 1)}),
    };

    std::array<UINT16, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };
    // cube->Setup(8, vertices.data(), 36, indices.data());
    
    auto sphere = std::make_shared<Mesh>();
    std::vector<Vertex> verts;
    std::vector<UINT16> triangles;
    sphereGenerate(verts, triangles, 10, 20, 20);
    sphere->Setup(verts.size(), verts.data(), triangles.size(), triangles.data());
    renderer->scene->AddObject(sphere);
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

