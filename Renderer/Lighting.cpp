#include "Lighting.h"

#include "Renderer.h"

void PreLightPass(Light* lights[])
{
    
}

unique_ptr<RootSignature> Light::rootSignature = make_unique<RootSignature>();

void Light::LoadCommonAssets()
{
    RootSignature::InitParam param[] = {
        RootSignature::InitParam { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0 }
    };
    rootSignature->Initialize(_countof(param), param);
}

void Light::Initialize(ID3D12GraphicsCommandList* commandList)
{
    shadowMap->CreateEmpty(DXGI_FORMAT_R32_FLOAT, renderer->displayWidth, renderer->displayHeight);
    shadowMap->InitAsSRV(renderer->GetCbvSrvUavHeap());
}

void Light::Update(UINT frameIndex)
{
}

void Light::InputAssemble(ID3D12GraphicsCommandList* commandList)
{
    shadowMap->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    auto handle = shadowMap->GetCpuHandle();
    commandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
}

void Light::Render(ID3D12GraphicsCommandList* commandList)
{
}

void Light::PrePass(ID3D12GraphicsCommandList* commandList, set<shared_ptr<RenderItem>> objects)
{
}
