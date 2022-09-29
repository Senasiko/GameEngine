#pragma once
#include "Camera.h"
#include "RenderItem.h"
#include "Texture.h"
#include "../Core/pch.h"

class LightManager
{
    
    ComPtr<ID3D12GraphicsCommandList> lightCommandList;
    ComPtr<ID3D12CommandAllocator> lightCommandAllocator;

};

class Light: public RenderItem
{
    static unique_ptr<RootSignature> rootSignature;

    unique_ptr<Texture2D> shadowMap = make_unique<Texture2D>();
    unique_ptr<Camera> camera = make_unique<Camera>();

public:
    virtual ~Light() = default;
    static void LoadCommonAssets();
    virtual void Initialize(ID3D12GraphicsCommandList* commandList);
    virtual void Update(UINT frameIndex);
    virtual void InputAssemble(ID3D12GraphicsCommandList* commandList);
    virtual void Render(ID3D12GraphicsCommandList* commandList);
    virtual void PrePass(ID3D12GraphicsCommandList* commandList, set<shared_ptr<RenderItem>> objects);
};

class DirectLight: public Light
{
public:
    
};
