#include "SceneEntity.h"

namespace Coco
{
    List<EntityComponent*> SceneEntity::GetComponents() const
    {
        List<EntityComponent*> components;

        for (const Managed<EntityComponent>& component : _components)
            components.Add(component.get());

        return components;
    }
}
