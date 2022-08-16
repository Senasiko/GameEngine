#include "RootSignature.h"
#include "Helper.h"
#include "Renderer.h"

void RootSignature::Initialize(UINT num, InitParam params[])
{
    descNum = num;
    CD3DX12_DESCRIPTOR_RANGE1* ranges = new CD3DX12_DESCRIPTOR_RANGE1[num];
    CD3DX12_ROOT_PARAMETER1* rootParameters = new CD3DX12_ROOT_PARAMETER1[num];
    UINT index = 0;
    for (UINT i = 0; i < num; i++)
    {
        switch (params[i].rangeType)
        {
            case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: ranges[index].Init(params[i].rangeType, params[i].numDescriptors, params[i].baseShaderRegister); break;
            default: ranges[index].Init(params[i].rangeType, params[i].numDescriptors, params[i].baseShaderRegister, params[i].registerSpace, params[i].flags, params[i].offsetInDescriptorsFromTableStart);
        }
        rootParameters[index].InitAsDescriptorTable(1, &ranges[index], D3D12_SHADER_VISIBILITY_ALL);
        index++;
    }
    
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(num, rootParameters, 0, nullptr,
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error));
    ThrowIfFailed(renderer->m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                          IID_PPV_ARGS(&rootSignature)));

}

void RootSignature::SetGraphicsRootSignature(DescBindable* descs[], ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    for (UINT i = 0; i < descNum; i++)
    {
        commandList->SetGraphicsRootDescriptorTable(i, descs[i]->GetGpuHandle());
    }
}
