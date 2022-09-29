#pragma once
#include "../Core/pch.h"
#include "Lighting.h"
#include "RenderItem.h"


class SceneTexture
{
    unique_ptr<Texture2D> gBufferBaseColor = make_unique<Texture2D>();
    unique_ptr<Texture2D> gBufferDepth = make_unique<Texture2D>();
    // unique_ptr<Texture2D> gBufferC = make_unique<Texture2D>();
public:
    void Initialize(RtvDescriptorHeap* heap, DsvDescriptorHeap* dsvHeap);

    D3D12_CPU_DESCRIPTOR_HANDLE* GetRTVCpuHandle() const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle[] = {
            gBufferBaseColor->GetCpuHandle(),
        };
        return handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* GetDSVCpuHandle() const
    {
        auto handle = gBufferDepth->GetCpuHandle();
        return &handle;
    }

    static UINT GetRenderTargetNum()
    {
        return 1;
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
