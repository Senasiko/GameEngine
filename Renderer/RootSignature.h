#pragma once
#include "../Core/pch.h"

class DescBindable
{
public:
    virtual ~DescBindable() = default;
    virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() = 0;
};


class RootSignature
{
    UINT descNum = 0;
    ComPtr<ID3D12RootSignature> rootSignature;
public:
    struct InitParam
    {
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
        UINT numDescriptors;
        UINT baseShaderRegister;
        UINT registerSpace = 0;
        D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        
    };
    ComPtr<ID3D12RootSignature>* GetRootSignatureAddressOf()
    {
        return &rootSignature;
    }
    
    ID3D12RootSignature* GetRootSignature()
    {
        return rootSignature.Get();
    }

    void Initialize(UINT num, InitParam params[]);
    void SetGraphicsRootSignature(DescBindable* descs[], ID3D12GraphicsCommandList* commandList);
};

// class MeshShaderParam : public RootSignature
// {
//     struct Param
//     {
//         static 
//         // ConstantBuffer* buffer;
//     };
//
//     void Initialize()
//     {
//         
//     }
//     void SetGraphicsRootSignature(Param param, ID3D12GraphicsCommandList* commandList)
//     {
//         
//         commandList->SetGraphicsRootSignature(, );
//     }
// };
//
// unique_ptr<MeshShaderParam> rootSignature;
//
//
//
// struct RootSignatureParam
// {
//     
// };