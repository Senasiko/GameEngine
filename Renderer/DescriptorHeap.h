#pragma once
#include "../Core/pch.h"

struct DescriptorHandle;

class DescriptorHeap
{
    D3D12_DESCRIPTOR_HEAP_TYPE heapType;
    D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag;
    ComPtr<ID3D12DescriptorHeap> heap;
    queue<UINT> freeHandle = {};
    UINT maxSize = 256;
    UINT count = 0;

    virtual void ReCreateDesc(UINT index) = 0;

    void ReAllocator();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleStart() const
    {
        return heap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandleStart() const
    {
        return heap->GetGPUDescriptorHandleForHeapStart();
    }
    
public:
    DescriptorHeap() = delete;
    DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag): heapType(heapType), heapFlag(heapFlag)
    {
    }
    virtual ~DescriptorHeap()
    {
    }
    void Initialize();
    UINT Allocator();
    UINT GetDescriptorIncrementSize() const;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(GetCpuHandleStart(), static_cast<INT>(index), GetDescriptorIncrementSize());
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const
    {
        // auto handle = GetGpuHandleStart();
        // CD3DX12_GPU_DESCRIPTOR_HANDLE start(handle);
        // return start.Offset(index, GetDescriptorIncrementSize());
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(GetGpuHandleStart(), static_cast<INT>(index), GetDescriptorIncrementSize());
    }

    ID3D12DescriptorHeap* GetHeap() const
    {
        return heap.Get();
    }


    void Release(UINT index)
    {
        freeHandle.push(index);
    }
};


struct DescriptorHandle
{
public:
    DescriptorHeap* heap;
    UINT index;
    DescriptorHandle() = default;
    DescriptorHandle(DescriptorHandle&& h) = delete;
    DescriptorHandle(DescriptorHandle& h) = delete;
    DescriptorHandle(DescriptorHandle* h) = delete;
    DescriptorHandle(DescriptorHeap* heap, UINT index): heap(heap), index(index)
    {
    }

    ~DescriptorHandle()
    {
        if (heap)
        {
            heap->Release(index);
        }
    }

    UINT GetIndex() const { return index; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle()
    {
        return heap->GetCpuHandle(index);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle()
    {
        return heap->GetGpuHandle(index);
    }
};


class CbvSrvUavDescriptorHeap : public DescriptorHeap
{
    enum SubHeapType
    {
        CBV = 0,  
        SRV = 1,  
        UAV = 2,  
    };
    struct SubDesc
    {
        SubHeapType type;
        ID3D12Resource* resource;
        union
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        };
    };
    map<UINT, SubDesc> descMap = {};
    void ReCreateDesc(UINT index) override
    {
        switch (descMap[index].type)
        {
            case CBV: AllocatorCBV(descMap[index].cbvDesc.BufferLocation, descMap[index].cbvDesc.SizeInBytes); break;
            case SRV: AllocatorSRV(descMap[index].resource, &descMap[index].srvDesc); break;
            case UAV: AllocatorUAV(descMap[index].resource, &descMap[index].uavDesc); break;
            default:;
        }
    }

    UINT AllocatorCBV(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes);
    UINT AllocatorSRV(ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
    UINT AllocatorUAV(ID3D12Resource* pResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);
public:
    CbvSrvUavDescriptorHeap(): DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {}
    ~CbvSrvUavDescriptorHeap() override = default;
    
    void CreateCBV(DescriptorHandle* handle, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes)
    {
        auto index = AllocatorCBV(bufferLocation, sizeInBytes);
        handle->index = index;
        handle->heap = this;
    }
    void CreateSRV(DescriptorHandle* handle, ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
    {
        
        auto index = AllocatorSRV(pResource, desc);
        handle->index = index;
        handle->heap = this;
    }
    void CreateUAV(DescriptorHandle* handle, ID3D12Resource* pResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
    {
        auto index = AllocatorUAV(pResource, desc);
        handle->index = index;
        handle->heap = this;
    }
};

class RtvDescriptorHeap: public DescriptorHeap
{
    map<UINT, ID3D12Resource*> resourceMap = {};
public:
    RtvDescriptorHeap(): DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {}
    ~RtvDescriptorHeap() override = default;
    void ReCreateDesc(UINT index) override;
    void AllocatorRTV(DescriptorHandle* handle, ID3D12Resource* pResource, D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc);
};

class DsvDescriptorHeap: public DescriptorHeap
{
    map<UINT, ID3D12Resource*> resourceMap = {};
public:
    DsvDescriptorHeap(): DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {}
    ~DsvDescriptorHeap() override = default;
    void ReCreateDesc(UINT index) override;
    void AllocatorDSV(DescriptorHandle* handle, ID3D12Resource* pResource);
};