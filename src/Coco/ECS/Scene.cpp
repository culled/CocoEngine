//
// Created by cullen on 4/1/26.
//

#include "Scene.h"

#include "ECSService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Scene, Resource);

    Scene::Scene(Engine* engine, uint64 id) :
        Resource(engine, id),
        _rootEntities()
    {}

    Scene::~Scene()
    {
        if (auto ecs = _engine->GetService<ECSService>())
        {
            while (!_rootEntities.IsEmpty())
            {
                ecs->DestroyEntityImmediate(_rootEntities.Front());
            }
        }

        _rootEntities.Clear(true);
    }

    Entity Scene::CreateEntity(const char* name, const UUID& parentID)
    {
        if (auto ecs = _engine->GetService<ECSService>())
            return ecs->CreateEntity(name, *this, parentID);

        return Entity();
    }
} // Coco