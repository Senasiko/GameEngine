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
    void Create(D3D12_RESOURCE_DESC* desc);
    void Upload(ID3D12GraphicsCommandList* commandList, UINT RowPitch, UINT SlicePitch, void* data);
    
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle()
    {
        return handle->GetGpuHandle();
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
    void CreateSRV(CbvSrvUavDescriptorHeap* heap) const;
};

