#include "Lighting.h"

#include "Renderer.h"

unique_ptr<RootSignature> Light::rootSignature = make_unique<RootSignature>();
ComPtr<ID3D12PipelineState> Light::pso = nullptr;
BOOL Light::bStaticInitialized = FALSE;

void Light::LoadCommonAssets()
{
    RootSignature::InitParam param[] = {
        RootSignature::InitParam { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0 },
        RootSignature::InitParam { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0 },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, Sampler::SamplerCount, 0 },
    };
    rootSignature->Initialize(_countof(param), param);

    
    // pso
    const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = vertexLayout;
    inputLayoutDesc.NumElements = _countof(vertexLayout);

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.pRootSignature = rootSignature->GetRootSignature();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(LightShader::getVertexShaderStream(), LightShader::getVertexShaderSize());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(LightShader::getPixelShaderStream(), LightShader::getPixelShaderSize());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;

    ThrowIfFailed(renderer->m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));
    NAME_D3D12_OBJECT(pso, TEXT("lightPso"));

}

void Light::Initialize(ID3D12GraphicsCommandList* commandList)
{
    if (!bStaticInitialized)
    {
        LoadCommonAssets();
        bStaticInitialized = TRUE;
    }
    shadowMap->CreateEmpty(DXGI_FORMAT_R32_FLOAT, renderer->displayWidth, renderer->displayHeight);
    shadowMap->InitAsDsV(renderer->GetDsvHeap(), renderer->displayWidth, renderer->displayHeight);

    array<float, 24> v = {-100.0f, -100, -100, -104, 105, -106, 107, 108, -109, 110, -111, -112, -101.0f, -102, 103, -104, 105, 106, 107, 108, 109, 110, -111, 112};
    array<UINT16, 36> i = {
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
    vertexBuffer->Create(commandList, &v, v.size() / 3, sizeof(float) * 3);
    indexBuffer->Create(commandList, &i, i.size());
    constantBuffer->Create(renderer->GetCbvSrvUavHeap(), sizeof(XMMATRIX));
    bIsInitialized = TRUE;
}

void Light::Update(UINT frameIndex)
{
    auto martix = GetWorldMatrix();
    constantBuffer->Update(&martix);
}

void Light::InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture)
{
    commandList->SetPipelineState(pso.Get());
    DescBindable* descs[] = {
        view,
        constantBuffer.get(),
        sceneTexture,
        renderer->GetSampler(),
    };
    rootSignature->SetGraphicsRootSignature(descs, commandList);
    vertexBuffer->Set(commandList);
    indexBuffer->Set(commandList);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Light::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->DrawIndexedInstanced(indexBuffer->GetIndexCount(), 1, 0, 0, 0);
}

void Light::PrePass(ID3D12GraphicsCommandList* commandList, set<shared_ptr<RenderItem>> objects)
{
}
