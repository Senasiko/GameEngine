#include "DefaultBuffer.h"
#include "Helper.h"
#include "Renderer.h"

void DefaultBuffer::Create(ID3D12GraphicsCommandList* commandList, const void* data, UINT16 size)
{
    byteSize = size;
    CD3DX12_HEAP_PROPERTIES uploadProp(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_HEAP_PROPERTIES defaultProp(D3D12_HEAP_TYPE_DEFAULT);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &uploadProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&bufferUpload)));
    NAME_D3D12_OBJECT(bufferUpload, STRING_TO_LPCWSTR(name));

    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &defaultProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&buffer)));
    NAME_D3D12_OBJECT(buffer, STRING_TO_LPCWSTR(name));

    D3D12_SUBRESOURCE_DATA resourceData = {};
    resourceData.pData = data;
    resourceData.RowPitch = byteSize;
    resourceData.SlicePitch = resourceData.RowPitch;
    UpdateSubresources(commandList, buffer.Get(), bufferUpload.Get(), 0, 0, 1, &resourceData);
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    commandList->ResourceBarrier(1, &transition);

}
