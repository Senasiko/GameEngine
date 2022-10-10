#include "ConstantBuffer.h"
#include "Helper.h"
#include "Renderer.h"

void ConstantBuffer::Create(CbvSrvUavDescriptorHeap* heap, UINT16 size)
{
    byteSize = CalcConstantByteSize(size);
    CD3DX12_HEAP_PROPERTIES uploadProp(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &uploadProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer)));
    NAME_D3D12_OBJECT(buffer, STRING_TO_LPCWSTR(name));

    auto range = CD3DX12_RANGE(0, byteSize);
    ThrowIfFailed(buffer->Map(0, &range, &memory));
    heap->CreateCBV(handle.get(), buffer->GetGPUVirtualAddress(), byteSize);
}


void ConstantBuffer::Update(void* data)
{
    memcpy(memory, data, byteSize);
}
