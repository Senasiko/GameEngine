#pragma once
#include "../Core/pch.h"
#include "Texture.h"
#include "DefaultBuffer.h"
#include "RenderItem.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Renderer.h"
#include "RootSignature.h"

#include SHADER_FILE(MeshVS)
#include SHADER_FILE(MeshPS)

GLOBAL_GRAPHIC_SHADER(MeshShader, MeshVS, MeshPS)

class Mesh : public RenderItem
{
    DECLARE_RENDER_ITEM(Mesh)
    unique_ptr<VertexBuffer> vertexBuffer = make_unique<VertexBuffer>();
    unique_ptr<IndexBuffer> indexBuffer = make_unique<IndexBuffer>();
    array<unique_ptr<ConstantBuffer>, FrameCount> constantBuffer = {};
    BOOL bInitialized = FALSE;

    void LoadAssets(ID3D12GraphicsCommandList* commandList);
    ID3D12PipelineState* GetPso() override
    {
        return pso.Get();
    }

    Transform transform = {XMVectorSet(0, 0, 0, 0), XMQuaternionRotationRollPitchYaw(0, 0, 0)};
public:
    Mesh() = default;
    static void LoadCommonAssets(ID3D12GraphicsCommandList* commandList);
    static ComPtr<ID3D12PipelineState> pso;
    static unique_ptr<RootSignature> rootSignature;
    static unique_ptr<Texture2D> baseColor;
    static BOOL bStaticInitialized;
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Update(UINT frameIndex) override;
    void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view) override;
    void Render(ID3D12GraphicsCommandList* commandList) override;
    BOOL IsInitialized() override { return bInitialized; };

    Transform* GetTransform() override
    {
        return &transform;
    }
};
