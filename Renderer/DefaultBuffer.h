#pragma once
#include "../Core/pch.h"

class DefaultBuffer
{
protected:
    ComPtr<ID3D12Resource> buffer;
    ComPtr<ID3D12Resource> bufferUpload;
    UINT16 byteSize = 0;
    string name = "buffer";
public:
    DefaultBuffer() = default;
    DefaultBuffer(string name): name(name) {}
    ~DefaultBuffer() = default;
    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 byteSize);
};

class VertexBuffer: public DefaultBuffer
{
    friend DefaultBuffer;
    D3D12_VERTEX_BUFFER_VIEW bufferView = {};
public:
    VertexBuffer() = default;
    VertexBuffer(string name): DefaultBuffer(name) {}
    ~VertexBuffer() = default;

    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 count, UINT16 strideInBytes)
    {
        UINT16 size = count * strideInBytes;
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
    UINT16 indexCount = 0;
public:
    IndexBuffer() = default;
    IndexBuffer(string name): DefaultBuffer(name) {}
    ~IndexBuffer() = default;

    void Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 count)
    {
        indexCount = count;
        UINT16 size = count * sizeof(UINT16);
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
        return indexCount;
    }
};
