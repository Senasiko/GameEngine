#pragma once
#include "Camera.h"
#include "../Core/pch.h"

class Light
{
    ComPtr<ID3D12Resource> shadowMap;
    unique_ptr<Camera> camera = make_unique<Camera>();
    unique_ptr<RootSignature> rootSignature = make_unique<RootSignature>();

public:
    void Initialize();
};
