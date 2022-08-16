#include "Sampler.h"
#include "Helper.h"
#include "Renderer.h"

void Sampler::Initialize()
{
    
    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
    samplerHeapDesc.NumDescriptors = SamplerCount;     
    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(renderer->m_device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&heap)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandle(heap->GetCPUDescriptorHandleForHeapStart());

    auto descSize = renderer->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    D3D12_SAMPLER_DESC wrapSamplerDesc = {};
    wrapSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    wrapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    wrapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    wrapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    wrapSamplerDesc.MinLOD = 0;
    wrapSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    wrapSamplerDesc.MipLODBias = 0.0f;
    wrapSamplerDesc.MaxAnisotropy = 1;
    wrapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    wrapSamplerDesc.BorderColor[0] = wrapSamplerDesc.BorderColor[1] = wrapSamplerDesc.BorderColor[2] = wrapSamplerDesc.BorderColor[3] = 0;
    renderer->m_device->CreateSampler(&wrapSamplerDesc, samplerHandle);

    samplerHandle.Offset(descSize);

    D3D12_SAMPLER_DESC clampSamplerDesc = {};
    clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    clampSamplerDesc.MipLODBias = 0.0f;
    clampSamplerDesc.MaxAnisotropy = 1;
    clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    clampSamplerDesc.BorderColor[0] = clampSamplerDesc.BorderColor[1] = clampSamplerDesc.BorderColor[2] = clampSamplerDesc.BorderColor[3] = 0;
    clampSamplerDesc.MinLOD = 0;
    clampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    renderer->m_device->CreateSampler(&clampSamplerDesc, samplerHandle);
}

void Sampler::Set(ID3D12GraphicsCommandList* commandList, UINT index)
{
    commandList->SetGraphicsRootDescriptorTable(index, heap->GetGPUDescriptorHandleForHeapStart());
}
