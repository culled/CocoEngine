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
    /// @brief A container for a set of entities
    class Scene : public Resource
    {
        friend class EntityStorage;

        DECLARE_RTTI_TYPE(Scene)

    public:
        Scene(Engine* engine, uint64 id);
        ~Scene();

        /// @brief Creates an entity in this scene
        /// @param name The name of the entity
        /// @param parentID The ID of the entity that will parent the new entity, or UUID::Nil for the new entity to not have a parent
        /// @return The created entity
        Entity CreateEntity(const char* name, const UUID& parentID = UUID::Nil);

        /// @brief Creates and returns a view that can be used to iterate over entities containing at least all the given types of components
        /// @tparam FirstComponent The first component type. For performance reasons, use the least common component type here
        /// @tparam AdditionalComponents The other required component types
        /// @param onlyActiveEntities If true, only active entities will be returned
        /// @return A view over the entities with all the given component types
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