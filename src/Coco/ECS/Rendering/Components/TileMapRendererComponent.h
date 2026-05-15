//
// Created by cullen on 5/4/26.
//

#ifndef COCOENGINE_TILEMAPRENDERERCOMPONENT_H
#define COCOENGINE_TILEMAPRENDERERCOMPONENT_H
#include "Coco/Core/Math/Rect.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/ECS/EntityComponent.h"
#include "Coco/Rendering/2D/Tilemap/TileMap.h"

namespace Coco
{
    struct TileMapRendererComponent : public EntityComponent
    {
        SharedPtr<TileMap> Map;
        uint32 DefaultTileID;

        TileMapRendererComponent(const UUID& ownerID, SharedPtr<TileMap> map);

        void CallForVisibleTiles(const Rect& localViewport, const std::function<void(const TileMapCell&)>& callbackFunction) const;
    };
} // Coco

#endif //COCOENGINE_TILEMAPRENDERERCOMPONENT_H