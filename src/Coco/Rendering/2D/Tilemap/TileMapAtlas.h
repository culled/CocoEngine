//
// Created by cullen on 4/11/26.
//

#ifndef COCOENGINE_TILEMAPATLAS_H
#define COCOENGINE_TILEMAPATLAS_H
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Rendering/Texture.h"

namespace Coco
{
    class TileMapAtlas
    {
    public:
        TileMapAtlas(SharedPtr<Texture> atlasTexture, uint32 cellsX, uint32 cellsY);

        SharedPtr<Texture> GetTexture() const { return _atlasTexture; }
        Vector4 GetCellSlice(uint32 cellID) const;
        uint32 GetWidth() const { return _cellsX; }
        uint32 GetHeight() const { return _cellsY; }
        uint32 GetCellID(uint32 x, uint32 y) const;

    private:
        SharedPtr<Texture> _atlasTexture;
        uint32 _cellsX;
        uint32 _cellsY;
    };
} // Coco

#endif //COCOENGINE_TILEMAPATLAS_H