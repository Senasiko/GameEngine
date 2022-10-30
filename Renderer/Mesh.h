#pragma once
#include "../Core/pch.h"
#include "Texture.h"
#include "DefaultBuffer.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Renderer.h"
#include "PipelineState.h"

#include SHADER_FILE(MeshVS)
#include SHADER_FILE(MeshPS)
#include SHADER_FILE(PreMeshPS)

GLOBAL_GRAPHIC_SHADER_WITH_PRE(MeshShader, MeshVS, MeshPS, PreMeshPS)

class Mesh : public RenderItem
{
    DECLARE_RENDER_ITEM(Mesh)
    unique_ptr<VertexBuffer> vertexBuffer = make_unique<VertexBuffer>("meshVertexBuffer");
    unique_ptr<IndexBuffer> indexBuffer = make_unique<IndexBuffer>("meshIndexBuffer");
    array<unique_ptr<ConstantBuffer>, FrameCount> constantBuffer = {};
    BOOL bInitialized = FALSE;
    UINT vertexNum;
    Vertex* vertices;
    UINT indexNum;
    UINT16* indices;

    void LoadAssets(ID3D12GraphicsCommandList* commandList);

public:
    Mesh() = default;
    ~Mesh() override = default;
    static void LoadCommonAssets(ID3D12GraphicsCommandList* commandList);
    static unique_ptr<PipelineState> pso;
    static unique_ptr<PipelineState> prePso;
    static unique_ptr<Texture2D> baseColor;
    static BOOL bStaticInitialized;
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Update(UINT frameIndex) override;
    void Setup(UINT vertexNum, Vertex* vertex, UINT indexNum, UINT16* indices);
    void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture, BOOLEAN bIsPre) override;
    void Render(ID3D12GraphicsCommandList* commandList) override;
    BOOL IsInitialized() override { return bInitialized; };

};
