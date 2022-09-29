#include "Mesh.h"

BOOL Mesh::bStaticInitialized = FALSE;
ComPtr<ID3D12PipelineState> Mesh::pso = nullptr;
unique_ptr<RootSignature> Mesh::rootSignature = make_unique<RootSignature>();
unique_ptr<Texture2D> Mesh::baseColor = nullptr;

void Mesh::LoadCommonAssets(ID3D12GraphicsCommandList* commandList)
{
    if (bStaticInitialized) return;
    RootSignature::InitParam params[] = {
        RootSignature::InitParam { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, Sampler::SamplerCount, 0 },
    };
    rootSignature->Initialize(_countof(params), params);
    // pso
    const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
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
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(MeshShader::getVertexShaderStream(), MeshShader::getVertexShaderSize());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(MeshShader::getPixelShaderStream(), MeshShader::getPixelShaderSize());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;

    ThrowIfFailed(renderer->m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));
    NAME_D3D12_OBJECT(pso);

    
    baseColor = make_unique<Texture2D>();
    baseColor->Create(commandList, "cube.png");
    baseColor->InitAsSRV(renderer->GetCbvSrvUavHeap());
}

void Mesh::LoadAssets(ID3D12GraphicsCommandList* commandList)
{
    const std::array<Vertex, 8> vertices =
    {
        Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Black), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Red), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Blue), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Yellow), XMFLOAT2(1, 1)}),
        Vertex({XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Cyan), XMFLOAT2(0, 0)}),
        Vertex({XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Magenta), XMFLOAT2(1, 1)}),
    };


    PIXBeginEvent(commandList, 0, "Mesh: upload vertex buffer");
    vertexBuffer->Create(commandList, &vertices, vertices.size() * sizeof(Vertex), sizeof(Vertex));
    PIXEndEvent(commandList);

    const std::array<UINT16, 36> indices =
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
    PIXBeginEvent(commandList, 0, "Mesh: upload index buffer");
    indexBuffer->Create(commandList, &indices, indices.size() * sizeof(UINT16));
    
    PIXEndEvent(commandList);
}


void Mesh::Initialize(ID3D12GraphicsCommandList* commandList)
{
    transform = {XMVectorSet(0, 0, 0, 0), XMQuaternionRotationRollPitchYaw(0, 0, 0)};

    if (!bStaticInitialized)
    {
        LoadCommonAssets(commandList);
        bStaticInitialized = TRUE;
    }
    if (!bInitialized)
    {
        LoadAssets(commandList);
        bInitialized = TRUE;
    }
}

void Mesh::Update(UINT frameIndex)
{
    if (constantBuffer[frameIndex] == nullptr)
    {
        constantBuffer[frameIndex] = make_unique<ConstantBuffer>();
        constantBuffer[frameIndex]->Create(renderer->GetCbvSrvUavHeap(), sizeof(XMMATRIX));
    }
    Rotate(0, 0.02, 0);
    auto martix = GetWorldMatrix();
    constantBuffer[frameIndex]->Update(&martix);
}

void Mesh::InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view)
{
    PIXBeginEvent(commandList, 0, "Mesh Render");
    commandList->SetPipelineState(pso.Get());
    DescBindable* descs[] = {
        view,
        constantBuffer[frameIndex].get(),
        baseColor.get(),
        renderer->GetSampler(),
    };
    rootSignature->SetGraphicsRootSignature(descs, commandList);

    vertexBuffer->Set(commandList);
    indexBuffer->Set(commandList);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->DrawIndexedInstanced(indexBuffer->GetIndexCount(), 1, 0, 0, 0);
    PIXEndEvent(commandList);
}
