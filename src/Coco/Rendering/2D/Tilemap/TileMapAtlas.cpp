//
// Created by cullen on 4/11/26.
//

#include "TileMapAtlas.h"

namespace Coco
{
    TileMapAtlas::TileMapAtlas(SharedPtr<Texture> atlasTexture, uint32 cellsX, uint32 cellsY) :
        _atlasTexture(std::move(atlasTexture)),
        _cellsX(cellsX),
        _cellsY(cellsY)
    {}

    Vector4 TileMapAtlas::GetCellSlice(uint32 cellID) const
    {
        float x = static_cast<float>(cellID % _cellsX);
        float y = static_cast<float>(cellID / _cellsX);

        float u = 1.0f / static_cast<float>(_cellsX);
        float v = 1.0f / static_cast<float>(_cellsY);

        return {x * u, y * v, u, v};
    }

    uint32 TileMapAtlas::GetCellID(uint32 x, uint32 y) const
    {
        return y * _cellsX + x;
    }
} // Coco