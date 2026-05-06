//
// Created by cullen on 4/11/26.
//

#ifndef COCOENGINE_TILEMAP_H
#define COCOENGINE_TILEMAP_H
#include "TileMapAtlas.h"

#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    struct TileMapCell
    {
        Vector2i Coordinates;
        uint32 TileID;

        TileMapCell(const Vector2i& coords, uint32 tileID);
    };

    class TileMap
    {
    public:
        TileMap(SharedPtr<TileMapAtlas> atlas);

        void SetCell(const Vector2i& coords, uint32 tileID);
        const TileMapCell* GetCell(const Vector2i& coords) const;

        SharedPtr<TileMapAtlas> GetAtlas() const { return _atlas; }

    private:
        SharedPtr<TileMapAtlas> _atlas;
        Map<Vector2i, TileMapCell> _map;
    };
}

#endif //COCOENGINE_TILEMAP_H