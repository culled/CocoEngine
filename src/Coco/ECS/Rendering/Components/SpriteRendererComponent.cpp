//
// Created by cullen on 4/5/26.
//

#include "SpriteRendererComponent.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Mesh.h>

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/MeshUtils.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(SpriteRendererComponent, EntityComponent);

    uint64 SpriteRendererComponent::SpriteMeshID = Resource::InvalidID;

    SpriteRendererComponent::SpriteRendererComponent(const UUID& ownerEntityID) :
        EntityComponent(ownerEntityID),
        SpriteTexture(nullptr),
        TintColor(Color::White),
        Rows(1),
        Columns(1),
        AtlasCellIndex(0),
        FlipX(false),
        FlipY(false)
    {}

    SpriteRendererComponent::SpriteRendererComponent(const UUID& ownerEntityID, SharedPtr<Texture> spriteTexture,
    const Color& tintColor) :
        EntityComponent(ownerEntityID),
        SpriteTexture(spriteTexture),
        TintColor(tintColor),
        Rows(1),
        Columns(1),
        AtlasCellIndex(0),
        FlipX(false),
        FlipY(false)
    {}

    SharedPtr<Mesh> SpriteRendererComponent::GetOrCreateSpriteMesh()
    {
        Engine* engine = Engine::Get();
        ResourceManager* resourceManager = engine->GetResourceManager();

        if (SpriteMeshID == Resource::InvalidID)
        {
            SharedPtr<Mesh> spriteMesh = resourceManager->CreateResource<Mesh>("SpriteMesh", false);
            MeshUtils::CreateXYGrid(Vector2::One, Vector3::Zero, *spriteMesh, VertexChannelFlags::UV0);
            SpriteMeshID = spriteMesh->GetID();
            return spriteMesh;
        }

        return resourceManager->GetResourceAs<Mesh>(SpriteMeshID);
    }

    void SpriteRendererComponent::SetAtlas(uint32 columns, uint32 rows) noexcept
    {
        Columns = Math::Max(columns, static_cast<uint32>(1));
        Rows = Math::Max(rows, static_cast<uint32>(1));
        AtlasCellIndex = 0;
    }

    Vector4 SpriteRendererComponent::GetCurrentAtlasCellSlice() const noexcept
    {
        float sizeX = 1.0f / static_cast<float>(Columns);
        float sizeY = 1.0f / static_cast<float>(Rows);
        float offsetX = static_cast<float>(AtlasCellIndex % Columns) * sizeX;
        float offsetY = static_cast<float>(AtlasCellIndex / Columns) * sizeY;

        if (FlipX)
        {
            offsetX += sizeX;
            sizeX = -sizeX;
        }

        if (FlipY)
        {
            offsetY += sizeY;
            sizeY = -sizeY;
        }

        return {offsetX, offsetY, sizeX, sizeY};
    }
} // Coco