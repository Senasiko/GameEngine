#include "Mesh.h"

BOOL Mesh::bStaticInitialized = FALSE;
unique_ptr<PipelineState> Mesh::pso = make_unique<PipelineState>("mesh pso");
unique_ptr<PipelineState> Mesh::prePso = make_unique<PipelineState>("mesh prePso");
unique_ptr<Texture2D> Mesh::baseColor = nullptr;

void Mesh::LoadCommonAssets(ID3D12GraphicsCommandList* commandList)
{
    if (bStaticInitialized) return;
    RootSignature::InitParam params[] = {
        RootSignature::InitParam { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        // RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC },
        RootSignature::InitParam {  D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, Sampler::SamplerCount, 0 },
    };
    D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    pso->SetInputLayout(_countof(vertexLayout), vertexLayout);
    pso->SetRootSignature(_countof(params), params);
    pso->SetShaders(
        CD3DX12_SHADER_BYTECODE(MeshShader::getVertexShaderStream(), MeshShader::getVertexShaderSize()),
        CD3DX12_SHADER_BYTECODE(MeshShader::getPixelShaderStream(), MeshShader::getPixelShaderSize())
    );
    pso->SetRenderTarget(SceneTexture::GetRTVNum(), SceneTexture::GetRTVFormats().data());
    pso->Create();
    
    prePso->SetInputLayout(_countof(vertexLayout), vertexLayout);
    prePso->SetRootSignature(_countof(params), params);
    prePso->SetShaders(
        CD3DX12_SHADER_BYTECODE(MeshShader::getVertexShaderStream(), MeshShader::getVertexShaderSize()),
        CD3DX12_SHADER_BYTECODE(MeshShader::getPrePixelShaderStream(), MeshShader::getPrePixelShaderSize())
    );
    auto format = DXGI_FORMAT_UNKNOWN;
    prePso->SetRenderTarget(0, &format);
    prePso->Create();
    
    
    baseColor = make_unique<Texture2D>();
    baseColor->Create(commandList, "cube.png");
}

void Mesh::LoadAssets(ID3D12GraphicsCommandList* commandList)
{
    
    PIXBeginEvent(commandList, 0, "Mesh: upload vertex buffer");
    vertexBuffer->Create(commandList, vertices, vertexNum, sizeof(Vertex));
    PIXEndEvent(commandList);

    PIXBeginEvent(commandList, 0, "Mesh: upload index buffer");
    indexBuffer->Create(commandList, indices, indexNum);
    PIXEndEvent(commandList);
}


void Mesh::Initialize(ID3D12GraphicsCommandList* commandList)
{
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
    Rotate(0, 0, 0.5);

}

void Mesh::Update(UINT frameIndex)
{
    if (constantBuffer[frameIndex] == nullptr)
    {
        constantBuffer[frameIndex] = make_unique<ConstantBuffer>();
        constantBuffer[frameIndex]->Create(renderer->GetCbvSrvUavHeap(), sizeof(XMMATRIX));
    }
    Rotate(0, 0, 0.01);
    auto martix = GetWorldMatrix();
    constantBuffer[frameIndex]->Update(&martix);
}

void Mesh::Setup(UINT vertexNum, Vertex* vertices, UINT indexNum, UINT16* indices)
{
    this->vertexNum = vertexNum;
    this->vertices = vertices;
    this->indexNum = indexNum;
    this->indices = indices;
}

void Mesh::InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture, BOOLEAN bIsPre)
{
    PIXBeginEvent(commandList, 0, "Mesh Render");
    if (bIsPre)
    {
        DescBindable* descs[] = {
            view,
            constantBuffer[frameIndex].get(),
            renderer->GetSampler(),
        };
        prePso->InputAssemble(commandList, descs);
    } else
    {
        DescBindable* descs[] = {
            view,
            constantBuffer[frameIndex].get(),
            renderer->GetSampler(),
        };
        pso->InputAssemble(commandList, descs);
    }
    vertexBuffer->Set(commandList);
    indexBuffer->Set(commandList);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->DrawIndexedInstanced(indexBuffer->GetIndexCount(), 1, 0, 0, 0);
    PIXEndEvent(commandList);
}
