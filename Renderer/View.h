#pragma once
#include "Camera.h"
#include "ConstantBuffer.h"
#include "../Core/pch.h"
#include "RootSignature.h"

class View: public DescBindable
{
    struct ViewConstantBuffer
    {
        XMMATRIX viewMatrix;
        XMMATRIX martix;
        XMMATRIX worldMatrix;
        XMVECTOR position;
        FLOAT viewNear;
        FLOAT viewFar;
        LONG screenWidth;
        LONG screenHeight;
    };
    
    unique_ptr<Camera> camera = make_unique<Camera>();
    unique_ptr<ConstantBuffer> buffer = make_unique<ConstantBuffer>();
    unique_ptr<D3D12_VIEWPORT> viewport = make_unique<D3D12_VIEWPORT>();
    unique_ptr<D3D12_RECT> scissorRect = make_unique<D3D12_RECT>();

public:
    virtual ~View() = default;
    void Initialize(CbvSrvUavDescriptorHeap* heap) const
    {
        buffer->Create(heap, GetConstantSize());
        
        viewport->MinDepth = 0.0f;
        viewport->MaxDepth = 1.0f;

    }

    void RefreshViewSize() const
    {
        viewport->Width = static_cast<FLOAT>(camera->GetWidth());
        viewport->Height = static_cast<FLOAT>(camera->GetHeight());

        scissorRect->right = camera->GetWidth();
        scissorRect->bottom = camera->GetHeight();
    }
    
    void UpdateViewSize(LONG screenWidth, LONG screenHeight) const
    {
        camera->UpdateScreenSize(screenWidth, screenHeight);
        RefreshViewSize();
    }

    void UpdateCamera(Camera &newCamera)
    {
        camera = make_unique<Camera>(newCamera);
        RefreshViewSize();
    }

    void UpdateFromOther(View* other)
    {
        camera = make_unique<Camera>(other->camera.get());
        RefreshViewSize();
    }

    void UpdateBuffer()
    {
        ViewConstantBuffer data = {
            XMMatrixTranspose(camera->GetViewMatrix()),
            XMMatrixTranspose(camera->GetViewProjectionMatrix()),
            XMMatrixTranspose(camera->GetProjectionToWorldMatrix()),
            camera->GetPosition(),
            camera->GetNearZ(),
            camera->GetFarZ(),
            camera->GetWidth(),
            camera->GetHeight(),
        };
        buffer->Update(&data);
    }

    XMMATRIX GetViewProjectionMatrix()
    {
        return camera->GetViewProjectionMatrix();
    }
    
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() override
    {
        return buffer->GetGpuHandle();
    }

    XMFLOAT2 GetViewSize() const
    {
        return { static_cast<float>(camera->GetWidth()), static_cast<float>(camera->GetHeight()) };
    }

    D3D12_VIEWPORT* GetViewport()
    {
        return viewport.get();
    }

    D3D12_RECT* GetScissorRect()
    {
        return scissorRect.get();
    }
        
    static UINT GetConstantSize()
    {
        return sizeof(ViewConstantBuffer);
    }
};
