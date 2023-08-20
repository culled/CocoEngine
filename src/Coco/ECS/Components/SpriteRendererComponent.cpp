#include "SpriteRendererComponent.h"
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/MeshPrimitives.h>

namespace Coco::ECS
{
    const string _quadMeshResourceName = "Sprite Plane";
    Ref<Rendering::Mesh> SpriteRendererComponent::_defaultSpriteMesh = Ref<Rendering::Mesh>::Empty;

    SpriteRendererComponent::SpriteRendererComponent(const EntityID& owner) : EntityComponent(owner)
    {}

    SpriteRendererComponent::SpriteRendererComponent(const EntityID & owner, Ref<Rendering::Material> material, Ref<Rendering::Mesh> customMesh) :
        EntityComponent(owner),
        _material(material),
        _customMesh(customMesh)
    {}

    Ref<Rendering::Mesh> SpriteRendererComponent::GetMesh()
    {
        ResourceLibrary* library = Engine::Get()->GetResourceLibrary();

        if (!_defaultSpriteMesh.IsValid())
        {
            _defaultSpriteMesh = Rendering::MeshPrimitives::CreateXZPlane("Sprite Plane", Vector2(-1.0, 1.0), Vector3(0.5, 0, 0.5));
            _defaultSpriteMesh->UploadData();
        }

        if (_customMesh.IsValid())
            return _customMesh;

        return _defaultSpriteMesh;
    }
}
