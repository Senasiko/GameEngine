#pragma once
#include "Camera.h"
#include "DefaultBuffer.h"
#include "RenderItem.h"
#include "Texture.h"
#include "Shader.h"
#include "View.h"
#include "../Core/pch.h"

#include SHADER_FILE(LightVS)
#include SHADER_FILE(LightPS)

enum LIGHT_TYPE
{
    LIGHT_TYPE_DIRECTIONAL = 0,
    LIGHT_TYPE_POINT = 1,
};

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
    
    unique_ptr<VertexBuffer> vertexBuffer = make_unique<VertexBuffer>("LightVertexBuffer");
    unique_ptr<IndexBuffer> indexBuffer = make_unique<IndexBuffer>("LightIndexBuffer");
    unique_ptr<ConstantBuffer> constantBuffer = make_unique<ConstantBuffer>("LightConstantBuffer");
    unique_ptr<Texture2D> shadowMap = make_unique<Texture2D>();
    unique_ptr<Texture2D> depthTexture = make_unique<Texture2D>();
    unique_ptr<View> shadowView = make_unique<View>();

    struct ConstantStruct
    {
        XMMATRIX matrix;
        XMMATRIX shadowViewProjectionMatrix;
        XMFLOAT4 lightPosition;
        XMFLOAT2 shadowViewSize;
        LIGHT_TYPE lightType;
    };
    ConstantStruct constantStruct = {};
    
public:
    virtual ~Light() = default;
    static void LoadCommonAssets();
    virtual void Initialize(ID3D12GraphicsCommandList* commandList);
    virtual void Update(UINT frameIndex);
    virtual void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture, BOOLEAN bIsPre);
    virtual void Render(ID3D12GraphicsCommandList* commandList);
    virtual void PrePass(ID3D12GraphicsCommandList* commandList, set<shared_ptr<RenderItem>> objects);
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuHandle()
    {
        return shadowMap->GetCpuHandle();
    }

    View* GetShadowView()
    {
        return shadowView.get();
    }

    void TransitionForPrePass(ID3D12GraphicsCommandList* commandList)
    {
        shadowMap->TransitionView(TEXTURE2D_VIEW_DSV);
        shadowMap->Transition(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        // depthTexture->TransitionView(TEXTURE2D_VIEW_DSV);
        // depthTexture->Transition(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }
    
    void TransitionForLightPass(ID3D12GraphicsCommandList* commandList)
    {
        shadowMap->TransitionView(TEXTURE2D_VIEW_SRV);
        shadowMap->Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
    }

    static UINT GetDSVNum()
    {
        return 1;
    }
    
    // static array<DXGI_FORMAT, 1> GetRTVFormats()
    // {
    //     return { DXGI_FORMAT_R32_FLOAT };
    // }
};

class DirectLight: public Light
{
public:
    
};
