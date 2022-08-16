#pragma once
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <queue>
#include <set>
#include <Windows.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include "d3dx12.h"
#ifdef _DEBUG
    #include "../App/packages/WinPixEventRuntime.1.0.220124001/Include/WinPixEventRuntime/pix3.h"
    #include <dxgidebug.h>
#endif

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

#define DebugLog(info) std::cout << info << std::endl;
#define WarningLog(info) std::warning << info << std::endl;

