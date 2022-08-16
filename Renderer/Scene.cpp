#include "Scene.h"
#include "Mesh.h"

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

