#pragma once
#include "../Core/pch.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"

class ConstantBuffer: public DescBindable
{
    ComPtr<ID3D12Resource> buffer;
    UINT byteSize = 0;
    unique_ptr<DescriptorHandle> handle = make_unique<DescriptorHandle>();

    string name = "constantBuffer";
    void* memory = nullptr;
public:
    ConstantBuffer() = default;
    ConstantBuffer(string name): name(name) {}
    virtual ~ConstantBuffer()
    {
        if (this && buffer.Get() != nullptr)
        {
            auto range = CD3DX12_RANGE(0, byteSize);
            buffer->Unmap(0, &range);
            memory = nullptr;
        }
    }
    void Create(CbvSrvUavDescriptorHeap* heap, UINT16 byteSize);
    void Update(void* data);
    UINT GetByteSize()
    {
        return byteSize;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle()
    {
        return handle->GetGpuHandle();
    }

    static UINT CalcConstantByteSize(UINT byteSize)
    {
        return static_cast<UINT>(ceil(static_cast<float>(byteSize) / static_cast<float>(256))) * 256;
    }
};

// class ConstantBuffer
// {
// public:
//     
// };
