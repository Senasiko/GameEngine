#pragma once
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "../Core/pch.h"

class Texture: public DescBindable
{
protected:
    ComPtr<ID3D12Resource> texture;
    ComPtr<ID3D12Resource> textureUpload;
    ComPtr<ID3D12Resource> bufferUpload;
    unique_ptr<DescriptorHandle> handle = make_unique<DescriptorHandle>();

    D3D12_RESOURCE_STATES state;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    UINT pixelSize = 4; 

public:
    ~Texture()
    {
    }
    void Create(D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST);
    void Upload(ID3D12GraphicsCommandList* commandList, UINT RowPitch, UINT SlicePitch, void* data);
        
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle()
    {
        return handle->GetCpuHandle();
    }
    
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle()
    {
        return handle->GetGpuHandle();
    }
    
    void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES destState)
    {
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), state, destState);
        commandList->ResourceBarrier(1, &transition);
        state = destState; 
    }

    ID3D12Resource* GetResource()
    {
        return texture.Get();
    }
};

class Texture2D: public Texture
{
    friend Texture;
    UINT width = 1;
    UINT height = 1;
    UINT index = 0;
    UINT mipLevels = 1;
    unsigned char* data;

public:
    void CreateEmpty(DXGI_FORMAT format, UINT width, UINT height, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COPY_DEST)
    {
        this->format = format;
        this->width = width;
        this->height = height;
        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
        Texture::Create(&desc, resourceState);
    }
    void Create(ID3D12GraphicsCommandList* commandList, string filename, UINT MipLevels = 1)
    {
        mipLevels = MipLevels;
        ResolveFromFile(filename);

        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
        desc.MipLevels = mipLevels;
        Texture::Create(&desc);
        Upload(commandList, width * pixelSize, width * pixelSize * height, data);
    }
    void ResolveFromFile(string filename);
    
    void InitAsSRV(CbvSrvUavDescriptorHeap* heap) const;
    void InitAsRtv(RtvDescriptorHeap* heap, DXGI_FORMAT format, UINT width, UINT height);
    void InitAsDsV(DsvDescriptorHeap* heap, UINT width, UINT height);
};

