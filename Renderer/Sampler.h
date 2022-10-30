#pragma once
#include "RootSignature.h"
#include "../Core/pch.h"

class Sampler: public DescBindable
{
    ComPtr<ID3D12DescriptorHeap> heap;
public:
    virtual ~Sampler() = default;
    void Initialize();
    void Set(ID3D12GraphicsCommandList* commandList, UINT index);

    constexpr static UINT SamplerCount = 3;

    ID3D12DescriptorHeap* Getheap()
    {
        return heap.Get();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle()
    {
        return heap->GetGPUDescriptorHandleForHeapStart();   
    }

};
