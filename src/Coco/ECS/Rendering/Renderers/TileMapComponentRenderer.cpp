//
// Created by cullen on 5/11/26.
//

#include "TileMapComponentRenderer.h"

#include "Coco/ECS/Entity.h"
#include "Coco/ECS/Components/Transform2DComponent.h"
#include "Coco/ECS/Rendering/Components/CameraComponent.h"
#include "Coco/ECS/Rendering/Components/SpriteRendererComponent.h"
#include "Coco/ECS/Rendering/Components/TileMapRendererComponent.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"

namespace Coco
{
    void TileMapComponentRenderer::TilemapObjectData::SetDrawData(RenderContext& ctx) const
    {
        uint8 data[sizeof(Matrix4x4) + sizeof(Vector4) * 2];
        memcpy(data, &Model, sizeof(Matrix4x4));
        memcpy(data + sizeof(Matrix4x4), &Slice, sizeof(Vector4));
        memcpy(data + sizeof(Matrix4x4) + sizeof(Vector4), &TintColor, sizeof(Vector4));
        ctx.SetDrawData(data, sizeof(data), Span<const SharedPtr<Texture>>({SpriteTexture}));
    }

    void TileMapComponentRenderer::Render(const Entity& tilemap, const Entity& camera, RenderScene& renderScene)
    {
        if (!camera.HasComponent<CameraComponent>() || !camera.HasComponent<Transform2DComponent>() ||
            !tilemap.HasComponent<Transform2DComponent>() || !tilemap.HasComponent<TileMapRendererComponent>())
            return;

        auto cameraComp = camera.GetComponent<CameraComponent>();
        auto cameraTransform = camera.GetComponent<Transform2DComponent>();

        float aspect = renderScene.GetFrameSize().GetAspectRatio<float>();
        float halfSize = cameraComp->OrthographicCamera.Size * 0.5f;
        Rect localViewport(Vector2(-halfSize * aspect, -halfSize), Size(cameraComp->OrthographicCamera.Size * aspect, cameraComp->OrthographicCamera.Size));
        Rect globalViewport = cameraTransform->TransformRect(localViewport);

        auto tileMapRenderer = tilemap.GetComponent<TileMapRendererComponent>();
        auto tileMapTransform = tilemap.GetComponent<Transform2DComponent>();

        Rect tilemapViewport = tileMapTransform->InverseTransformRect(globalViewport);
        auto spriteMesh = SpriteRendererComponent::GetOrCreateSpriteMesh();
        tileMapRenderer->CallForVisibleTiles(tilemapViewport, [&](const TileMapCell& cellData)
        {
            TilemapObjectData tilemapObjectData;
            tilemapObjectData.Model = tileMapTransform->GlobalTransform;
            tilemapObjectData.Model.M14() += (static_cast<float>(cellData.Coordinates.X()) + 0.5f) * tileMapTransform->LocalScale.X();
            tilemapObjectData.Model.M24() += (static_cast<float>(cellData.Coordinates.Y()) + 0.5f) * tileMapTransform->LocalScale.Y();

            tilemapObjectData.TintColor = Color::White.AsVector4(false);
            tilemapObjectData.Slice = tileMapRenderer->Map->GetAtlas()->GetCellSlice(cellData.TileID);
            tilemapObjectData.SpriteTexture = tileMapRenderer->Map->GetAtlas()->GetTexture();

            uint64 objectID = Math::CombineHashes(ToHash(tilemap), static_cast<uint64>(cellData.Coordinates.X()), static_cast<uint64>(cellData.Coordinates.Y()));
            renderScene.StoreData(objectID, true, tilemapObjectData);
            renderScene.AddObject(objectID, 0, static_cast<float>(tileMapTransform->ZIndex), *spriteMesh, 0);
        });
    }
} // Coco