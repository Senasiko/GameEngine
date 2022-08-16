#pragma once
#include "../Core/pch.h"

class DefaultBuffer
{
protected:
    ComPtr<ID3D12Resource> buffer;
    ComPtr<ID3D12Resource> bufferUpload;
    UINT16 byteSize = 0;
public:
    ~DefaultBuffer() = default;
    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 byteSize);
};

class VertexBuffer: public DefaultBuffer
{
    friend DefaultBuffer;
    D3D12_VERTEX_BUFFER_VIEW bufferView = {};
public:
    ~VertexBuffer() = default;

    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 size, UINT16 strideInBytes)
    {
        DefaultBuffer::Create(commandList, data, size);
        bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
        bufferView.SizeInBytes = size;
        bufferView.StrideInBytes = strideInBytes;
    }

    void Set(ID3D12GraphicsCommandList* commandList)
    {
        commandList->IASetVertexBuffers(0, 1, &bufferView);
    }
};

class IndexBuffer: public DefaultBuffer
{
    friend DefaultBuffer;
    D3D12_INDEX_BUFFER_VIEW bufferView = {};
public:
    ~IndexBuffer() = default;

    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 size)
    {
        DefaultBuffer::Create(commandList, data, size);
        bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
        bufferView.SizeInBytes = size;
        bufferView.Format = DXGI_FORMAT_R16_UINT;
    }

    void Set(ID3D12GraphicsCommandList* commandList) const
    {
        commandList->IASetIndexBuffer(&bufferView);
    }

    UINT16 GetIndexCount() const
    {
        return byteSize / sizeof(UINT16);
    }
};
