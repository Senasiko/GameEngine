#pragma once
#include "Camera.h"
#include "ConstantBuffer.h"
#include "../Core/pch.h"
#include "RootSignature.h"

class View: public DescBindable
{
public:
    virtual ~View() = default;
    unique_ptr<Camera> camera = make_unique<Camera>();
    unique_ptr<ConstantBuffer> buffer = make_unique<ConstantBuffer>();

    void Initialize(CbvSrvUavDescriptorHeap* heap) const
    {
        buffer->Create(heap, GetConstantSize());
    }

    void Update() const
    {
        // XMVectorAdd(camera.get()->position, XMVectorSet(1, 1, 1, 1));
    }

    void UpdateFromOther(View* other)
    {
        camera = make_unique<Camera>(other->camera.get());
    }

    void UpdateBuffer(LONG screenWidth, LONG screenHeight)
    {
        auto matrix = camera->GetViewProjectionMatrix(screenWidth, screenHeight);
        buffer->Update(&matrix);
    }
        
    static UINT GetConstantSize()
    {
        return Camera::GetConstantSize();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() override
    {
        return buffer->GetGpuHandle();
    }
};
