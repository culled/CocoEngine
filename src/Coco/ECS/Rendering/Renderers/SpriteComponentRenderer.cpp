//
// Created by cullen on 5/11/26.
//

#include "SpriteComponentRenderer.h"

#include "Coco/ECS/Components/Transform2DComponent.h"
#include "Coco/ECS/Components/Transform3DComponent.h"
#include "Coco/ECS/Rendering/Components/SpriteRendererComponent.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/ECS/Scene.h"

namespace Coco
{
    void SpriteComponentRenderer::SpriteObjectData::SetDrawData(RenderContext& ctx) const
    {
        uint8 data[sizeof(Matrix4x4) + sizeof(Vector4) * 2];
        memcpy(data, &Model, sizeof(Matrix4x4));
        memcpy(data + sizeof(Matrix4x4), &Slice, sizeof(Vector4));
        memcpy(data + sizeof(Matrix4x4) + sizeof(Vector4), &TintColor, sizeof(Vector4));
        ctx.SetDrawData(data, sizeof(data), Span<const SharedPtr<Texture>>({SpriteTexture}));
    }

    void SpriteComponentRenderer::Render2D(const Entity& sprite, RenderScene& renderScene)
    {
        if (!sprite.HasComponent<Transform2DComponent>() || !sprite.HasComponent<SpriteRendererComponent>())
            return;

        auto transformComponent = sprite.GetComponent<Transform2DComponent>();
        auto spriteComponent = sprite.GetComponent<SpriteRendererComponent>();

        SpriteObjectData spriteData;
        spriteData.Model = transformComponent->GlobalTransform;
        spriteData.TintColor = spriteComponent->TintColor.AsVector4(false);
        spriteData.Slice = spriteComponent->GetCurrentAtlasCellSlice();
        spriteData.SpriteTexture = spriteComponent->SpriteTexture;

        uint64 objectID = ToHash(spriteComponent->OwnerID);
        renderScene.StoreData(objectID, true, spriteData);
        renderScene.AddObject(objectID, 0, static_cast<float>(transformComponent->ZIndex), *SpriteRendererComponent::GetOrCreateSpriteMesh(), 0);
    }

    void SpriteComponentRenderer::Render3D(const Entity& sprite, const Vector3& cameraPosition, RenderScene& renderScene)
    {
        if (!sprite.HasComponent<Transform3DComponent>() || !sprite.HasComponent<SpriteRendererComponent>())
            return;

        auto transformComponent = sprite.GetComponent<Transform3DComponent>();
        auto spriteComponent = sprite.GetComponent<SpriteRendererComponent>();

        SpriteObjectData spriteData;
        spriteData.Model = transformComponent->GlobalTransform;
        spriteData.TintColor = spriteComponent->TintColor.AsVector4(false);
        spriteData.Slice = spriteComponent->GetCurrentAtlasCellSlice();
        spriteData.SpriteTexture = spriteComponent->SpriteTexture;

        uint64 objectID = ToHash(spriteComponent->OwnerID);
        renderScene.StoreData(objectID, true, spriteData);

        float dist = (cameraPosition - transformComponent->GetGlobalPosition()).GetLengthSquared();
        renderScene.AddObject(objectID, 0, dist, *SpriteRendererComponent::GetOrCreateSpriteMesh(), 0);
    }
} // Coco