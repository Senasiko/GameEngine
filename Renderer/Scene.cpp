#include "Scene.h"
#include "Mesh.h"

void SceneTexture::Initialize(RtvDescriptorHeap* heap, DsvDescriptorHeap* dsvHeap)
{
    gBufferBaseColor->InitAsRtv(heap, DXGI_FORMAT_R32G32B32A32_FLOAT, renderer->displayWidth, renderer->displayHeight);
    // gBufferDepth->InitAsRtv(heap, DXGI_FORMAT_R32G32B32A32_FLOAT, renderer->displayWidth, renderer->displayHeight);
    gBufferDepth->InitAsDsV(dsvHeap, renderer->displayWidth, renderer->displayHeight);
    // gBufferBaseColor->CreateEmpty(DXGI_FORMAT_R32G32B32A32_FLOAT, renderer->displayWidth, renderer->displayHeight);
}

void Scene::AddObject(SceneItem item)
{
    string type = item->GetType();
    if (type.empty())
    {
        cout << "error" << endl;
    }
    if (!objectsMap.contains(type))
    {
        std::set<SceneItem> typeSet;
        typeSet.insert(item);
        objectsMap.insert(std::pair(type, typeSet));
    } else
    {
        objectsMap[type].insert(item);
    }
}

void Scene::RemoveObject(SceneItem item)
{
    
    auto type = item->GetType();
    if (objectsMap.contains(type))
    {
        auto typeSet = objectsMap[type];
        typeSet.erase(item);
        objectsMap.insert(std::pair(type, typeSet));
    }
}

void Scene::AddLight(shared_ptr<Light> light)
{
    lights.insert(light);
}

void Scene::RemoveLight(shared_ptr<Light> light)
{
    lights.erase(light);
}

