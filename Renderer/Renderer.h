#pragma once

#include "../Core/pch.h"
#include "../Core/WindowAdapter.h"

#include "FrameResource.h"
#include "RenderItem.h"
#include "Sampler.h"
#include "Scene.h"
#include "View.h"
constexpr int FrameCount = 3;

class Renderer
{

    UINT m_frameIndex = 0;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<IDXGISwapChain3> m_swapChain;
    
    unique_ptr<CbvSrvUavDescriptorHeap> cbvSrvUavHeap = make_unique<CbvSrvUavDescriptorHeap>();
    unique_ptr<RtvDescriptorHeap> rtvHeap = make_unique<RtvDescriptorHeap>();
    unique_ptr<Sampler> sampler = make_unique<Sampler>();
    unique_ptr<DsvDescriptorHeap> dsvHeap = make_unique<DsvDescriptorHeap>();


    HANDLE m_fenceEvent = nullptr;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue = 0;

    FrameResource* frameResources[FrameCount] = {};
    
    void WaitForGpu();
    void Render();
    void Present();
    void PrePass();
    void BasePass();
    void LightPass();

public:
    unique_ptr<Scene> scene;
    unique_ptr<View> view;
    ~Renderer()
    {
        for (auto i = 0; i < FrameCount; i++)
        {
            delete frameResources[i];
        }
    }
    ComPtr<ID3D12Device> m_device;
    static LONG displayWidth;
    static LONG displayHeight;
    void Initialize(LONG width, LONG height);
    void LoadCommonAssets();
    void LoadAssets();
    void Update();

    [[nodiscard]] RtvDescriptorHeap* GetRtvHeap() const
    {
        return rtvHeap.get();
    }
    
    [[nodiscard]] DsvDescriptorHeap* GetDsvHeap() const
    {
        return dsvHeap.get();
    }
    
    [[nodiscard]] Sampler* GetSampler() const
    {
        return sampler.get();
    }
    
    [[nodiscard]] ID3D12DescriptorHeap* GetSamplerHeap() const
    {
        return sampler->Getheap();
    }
    [[nodiscard]] CbvSrvUavDescriptorHeap* GetCbvSrvUavHeap() const
    {
        return cbvSrvUavHeap.get();
    }
    
    [[nodiscard]] IDXGISwapChain3* GetSwapChain() const
    {
        return m_swapChain.Get();
    }
};

extern const unique_ptr<Renderer> renderer;