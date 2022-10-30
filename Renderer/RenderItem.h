#pragma once
#include "../Core/pch.h"

class View;
class SceneTexture;

class RenderItem
{
public:
    struct Transform
    {
        XMVECTOR translate;
        XMVECTOR rotate;
    };
private:
    struct Transform;
    virtual ID3D12RootSignature* GetRootSignature() { return nullptr; };
    virtual ID3D12PipelineState* GetPso() { return nullptr; };
    Transform transform = {XMVectorSet(0, 0, 0, 1), XMQuaternionRotationRollPitchYaw(0, 0, 0)};
    
public:
    BOOL bIsInitialized = FALSE;

    virtual ~RenderItem() = default;

    virtual void Initialize(ID3D12GraphicsCommandList* commandList) = 0;
    virtual void Update(UINT frameIndex) = 0;
    virtual void InputAssemble(ID3D12GraphicsCommandList* commandList, UINT frameIndex, View* view, SceneTexture* sceneTexture, BOOLEAN bIsPre) = 0;
    virtual void Render(ID3D12GraphicsCommandList* commandList) = 0;
    virtual string GetType() = 0;

    virtual BOOL IsInitialized() { return bIsInitialized; };
    virtual Transform* GetTransform()
    {
        return &transform;   
    }
    virtual void SetTranslate(float x, float y, float z)
    {
        transform.translate = XMVectorSet(x, y, z, 1);
    }
    virtual void Rotate(float Pitch, float Yaw, float Roll)
    {
        Transform* transform = GetTransform();
        transform->rotate = XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll), transform->rotate);
    }
    virtual XMMATRIX GetWorldMatrix()
    {
        Transform* transform = GetTransform();
        return XMMatrixMultiplyTranspose(XMMatrixTranslationFromVector(transform->translate), XMMatrixRotationQuaternion(transform->rotate));
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
