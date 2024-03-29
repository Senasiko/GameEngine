﻿#pragma once
#include "../Core/pch.h"
#include "Lighting.h"
#include "RenderItem.h"


class SceneTexture: public DescBindable
{
   public:
    unique_ptr<Texture2D> gBufferBaseColor = make_unique<Texture2D>("gBufferBaseColor");
    unique_ptr<Texture2D> gBufferNormal = make_unique<Texture2D>("gBufferNormal");
    unique_ptr<Texture2D> gBufferDepth = make_unique<Texture2D>("gBufferDepth");
    // unique_ptr<Texture2D> gBufferC = make_unique<Texture2D>();
    void Initialize(RtvDescriptorHeap* heap, DsvDescriptorHeap* dsvHeap, CbvSrvUavDescriptorHeap* srvHeap);

    void TransitionForLightPass(ID3D12GraphicsCommandList* commandList) const
    {
        gBufferBaseColor->Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
        gBufferBaseColor->TransitionView(TEXTURE2D_VIEW_SRV);
        gBufferNormal->Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
        gBufferNormal->TransitionView(TEXTURE2D_VIEW_SRV);
        gBufferDepth->Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
        gBufferDepth->TransitionView(TEXTURE2D_VIEW_SRV);
    }
    
    void TransitionForRender(ID3D12GraphicsCommandList* commandList) const
    {
        gBufferBaseColor->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        gBufferBaseColor->TransitionView(TEXTURE2D_VIEW_RTV);
        gBufferNormal->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        gBufferNormal->TransitionView(TEXTURE2D_VIEW_RTV);
        gBufferDepth->Transition(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        gBufferDepth->TransitionView(TEXTURE2D_VIEW_DSV);
    }

    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 2> GetRTVCpuHandles() const
    {
        std::array handles = {
            gBufferBaseColor->GetCpuHandle(),
            gBufferNormal->GetCpuHandle(),
        };
        return handles;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuHandle() const
    {
        return gBufferDepth->GetCpuHandle();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() override
    {
        return gBufferBaseColor->GetGpuHandle();
    }

    static UINT GetGBufferNum()
    {
        return 3;
    }
    
    static UINT GetRTVNum()
    {
        return 2;
    }
    
    static array<DXGI_FORMAT, 2> GetRTVFormats()
    {
        return { DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM };
    }    
};

class Scene
{
public:
    using SceneItem = shared_ptr<RenderItem>;

    void AddObject(SceneItem item);
    void RemoveObject(SceneItem item);
    auto GetObjectsSetIter() { return objectsMap.begin(); };
    auto GetObjectsSet(string key) { return objectsMap[key]; };

    template<typename Callback>
    void ObjectIter(Callback cb)
    {
        auto iter = objectsMap.begin();
        while (iter != objectsMap.end())
        {
            auto typeSet = objectsMap[iter->first];
            auto setIter = typeSet.begin();
            while (setIter != typeSet.end())
            {
                cb(*setIter);
                ++setIter;
            }
            ++iter;

        }
    }

    void AddLight(shared_ptr<Light> light);
    void RemoveLight(shared_ptr<Light> light);

    template<typename Callback>
    void LightIter(Callback cb)
    {
        auto setIter = lights.begin();
        while (setIter != lights.end())
        {
            cb(*setIter);
            ++setIter;
        }
    }
    
private:
    map<string, set<SceneItem>> objectsMap = {};
    set<shared_ptr<Light>> lights = {};
};
