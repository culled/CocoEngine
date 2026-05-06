//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_SCENE_H
#define COCOENGINE_SCENE_H
#include "Entity.h"
#include "Coco/Core/Resources/Resource.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/UUID.h"
#include "EntityComponentView.h"

#include "Coco/Core/Engine.h"

namespace Coco
{
    class Scene : public Resource
    {
        friend class EntityStorage;

        DECLARE_RTTI_TYPE(Scene)

    public:
        Scene(Engine* engine, uint64 id);
        ~Scene();

        Entity CreateEntity(const char* name, const UUID& parentID = UUID::Nil);

        template<typename FirstComponent, typename ... AdditionalComponents>
        EntityComponentView<FirstComponent, AdditionalComponents ...> CreateComponentView(bool onlyActiveEntities)
        {
            return EntityComponentView<FirstComponent, AdditionalComponents ...>(_engine->GetService<ECSService>(), onlyActiveEntities, this);
        }

    private:
        Array<UUID> _rootEntities;
    };
} // Coco

#endif //COCOENGINE_SCENE_H