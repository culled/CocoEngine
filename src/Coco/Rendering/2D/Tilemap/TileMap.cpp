//
// Created by cullen on 4/11/26.
//

#include "TileMap.h"

namespace Coco
{
    TileMapCell::TileMapCell(const Vector2i& coords, uint32 tileID) :
        Coordinates(coords),
        TileID(tileID)
    {}

    TileMap::TileMap(SharedPtr<TileMapAtlas> atlas) :
        _atlas(std::move(atlas)),
        _map()
    {}

    void TileMap::SetCell(const Vector2i& coords, uint32 tileID)
    {
        if (auto existing = _map.TryGetValue(coords))
        {
            existing->TileID = tileID;
        }
        else
        {
            _map.Emplace(coords, coords, tileID);
        }
    }

    const TileMapCell* TileMap::GetCell(const Vector2i& coords) const
    {
        return _map.TryGetValue(coords);
    }
}
