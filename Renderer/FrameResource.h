#pragma once
#include "Camera.h"
#include "RenderItem.h"
#include "Scene.h"
#include "View.h"

class ConstantBuffer;
class Renderer;

class FrameResource
{
    // render items
    set<shared_ptr<RenderItem>> objectSet = {};
    unique_ptr<View> frameView = make_unique<View>();
    ComPtr<ID3D12Resource> dsBuffer;

    D3D12_VIEWPORT viewport; 
    D3D12_RECT scissorRect;
public:
    FrameResource(UINT frameIndex): frameIndex(frameIndex){}
    UINT frameIndex;
    UINT64 fenceValue = 0;
    ComPtr<ID3D12Resource> renderTarget;
    
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ComPtr<ID3D12CommandAllocator> commandAlloc;

    void Initialize();
    void Bind(Scene* scene);
    void Update(Scene* scene, View* view);
    void ResetCommandList();
    void RenderStart();
    void RenderObjects();
    void RenderEnd();

    [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const
    {
        return commandList.Get();
    }

    [[nodiscard]] View* GetFrameView() const
    {
        return frameView.get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle();
    D3D12_GPU_DESCRIPTOR_HANDLE GetRtvGpuHandle();
    
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle();
    D3D12_GPU_DESCRIPTOR_HANDLE GetDsvGpuHandle();
};
