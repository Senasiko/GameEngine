#pragma once
#include "Camera.h"
#include "DefaultBuffer.h"
#include "RenderItem.h"
#include "Texture.h"
#include "Shader.h"
#include "../Core/pch.h"

#include SHADER_FILE(LightVS)
#include SHADER_FILE(LightPS)

class LightManager
{
    
    ComPtr<ID3D12GraphicsCommandList> lightCommandList;
    ComPtr<ID3D12CommandAllocator> lightCommandAllocator;

};

GLOBAL_GRAPHIC_SHADER(LightShader, LightVS, LightPS);

class Light: public RenderItem
{
    DECLARE_RENDER_ITEM(Light)
    static unique_ptr<RootSignature> rootSignature;
    static ComPtr<ID3D12PipelineState> pso;
    static BOOL bStaticInitialized;
    
    unique_ptr<VertexBuffer> vertexBuffer = make_unique<VertexBuffer>("lightVertexBuffer");
    unique_ptr<IndexBuffer> indexBuffer = make_unique<IndexBuffer>("lightIndexBuffer");
    unique_ptr<ConstantBuffer> constantBuffer = make_unique<ConstantBuffer>();
    unique_ptr<Texture2D> shadowMap = make_unique<Texture2D>();
    unique_ptr<Camera> camera = make_unique<Camera>();

public:
    virtual ~Light() = default;
    static void LoadCommonAssets();
    virtual void Initialize(ID3D12GraphicsCommandList* commandList);
    virtual void Update(UINT frameIndex);
    virtual void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture);
    virtual void Render(ID3D12GraphicsCommandList* commandList);
    virtual void PrePass(ID3D12GraphicsCommandList* commandList, set<shared_ptr<RenderItem>> objects);
    virtual VertexBufferOption GetVertices()
    {
        float vertices[] = {1000.0f, 1000, 1000, 1000, 1000, 1000, 1000, 1000}; 
        return VertexBufferOption { vertices, _countof(vertices), sizeof(float) };
    }
    virtual IndexBufferOption GetIndices()
    {
        UINT16* indices =
        new UINT16[36] {
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
        return IndexBufferOption { indices, 36, sizeof(UINT16) };
    }
    D3D12_CPU_DESCRIPTOR_HANDLE GetShadowMapCpuHandle()
    {
        return shadowMap->GetCpuHandle();
    }
};

class DirectLight: public Light
{
public:
    
};
