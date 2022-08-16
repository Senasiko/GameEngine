#pragma once
#include "../Core/pch.h"
#include "Lighting.h"
#include "RenderItem.h"


class SceneTexture
{
public:
    ComPtr<ID3D12Resource> GBufferA;
    ComPtr<ID3D12Resource> GBufferB;
    ComPtr<ID3D12Resource> GBufferC;
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
    
private:
    map<string, set<SceneItem>> objectsMap = {};
    vector<Light> lights = {};
};
