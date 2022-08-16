#pragma once
#include "../Core/pch.h"

class FrameResource;
class View;

class RenderItem
{
    virtual ID3D12RootSignature* GetRootSignature() { return nullptr; };
    virtual ID3D12PipelineState* GetPso() { return nullptr; };
public:
    virtual ~RenderItem() = default;
    
    struct Transform
    {
        XMVECTOR translate;
        XMVECTOR rotate;
    };

    virtual BOOL IsInitialized() = 0;
    virtual void Initialize(ID3D12GraphicsCommandList* commandList) = 0;
    virtual void Update(UINT frameIndex) = 0;
    virtual void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view) = 0;
    virtual void Render(ID3D12GraphicsCommandList* commandList) = 0;
    virtual string GetType() = 0;

    virtual Transform* GetTransform() = 0;
    virtual void Rotate(float Pitch, float Yaw, float Roll)
    {
        Transform* transform = GetTransform();
        transform->rotate = XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll), transform->rotate);
    }
    virtual XMMATRIX GetWorldMatrix()
    {
        Transform* transform = GetTransform();
        return XMMatrixTranspose(XMMatrixMultiply(XMMatrixTranslationFromVector(transform->translate), XMMatrixRotationQuaternion(transform->rotate)));
    }
};



#define DECLARE_RENDER_ITEM(ClassName) \
    public: \
    string GetType() \
    {\
        return #ClassName;\
    }\
    private: \



#define BEGIN_RENDER_ITEM_CONSTANT_BUFFER(Parameters) \
    struct Parameters {\

#define END_RENDER_ITEM_CONSTANT_BUFFER() \
}\
