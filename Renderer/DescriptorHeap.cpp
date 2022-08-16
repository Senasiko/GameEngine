#include "DescriptorHeap.h"
#include "Renderer.h"

void DescriptorHeap::Initialize()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = heapType;
    desc.NodeMask = 0;
    desc.NumDescriptors = maxSize;
    renderer->m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
}

UINT DescriptorHeap::Allocator()
{
    UINT index = 0;
    if (count < maxSize)
    {
        index = count;
        count += 1;
    }
    else if (freeHandle.size() > 0)
    {
        index = freeHandle.front();
        freeHandle.pop();
    }
    else
    {
        ReAllocator();
        index = maxSize;
    }
    return index;
}

void DescriptorHeap::ReAllocator()
{
    ComPtr<ID3D12DescriptorHeap> newHeap;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = heapType;
    desc.NodeMask = 0;
    desc.NumDescriptors = maxSize * 2;
    renderer->m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&newHeap));

    for (UINT i = 0; i < maxSize; i++)
    {
        ReCreateDesc(i);
    }

    maxSize *= 2;
}

UINT DescriptorHeap::GetDescriptorIncrementSize() const
{
    return renderer->m_device->GetDescriptorHandleIncrementSize(heapType);
}

UINT CbvSrvUavDescriptorHeap::AllocatorCBV(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes)
{
    auto index = Allocator();
    D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
    desc.BufferLocation = bufferLocation;
    desc.SizeInBytes = sizeInBytes;
    renderer->m_device->CreateConstantBufferView(&desc, GetCpuHandle(index));
    SubDesc subDesc = {};
    subDesc.type = CBV;
    subDesc.cbvDesc = desc;
    descMap.insert_or_assign(index, subDesc);
    return index;
}

UINT CbvSrvUavDescriptorHeap::AllocatorSRV(ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    auto index = Allocator();
    renderer->m_device->CreateShaderResourceView(pResource, desc, GetCpuHandle(index));
    SubDesc subDesc = {};
    subDesc.type = SRV;
    subDesc.srvDesc = *desc;
    descMap.insert_or_assign(index, subDesc);
    return index;
}

UINT CbvSrvUavDescriptorHeap::AllocatorUAV(ID3D12Resource* pResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
{
    auto index = Allocator();
    renderer->m_device->CreateUnorderedAccessView(pResource, nullptr, desc, GetCpuHandle(index));
    SubDesc subDesc = {};
    subDesc.type = UAV;
    subDesc.uavDesc = *desc;
    descMap.insert_or_assign(index, subDesc);
    return index;
}