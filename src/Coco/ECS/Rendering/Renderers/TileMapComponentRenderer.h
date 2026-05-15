//
// Created by cullen on 5/11/26.
//

#ifndef COCOENGINE_TILEMAPCOMPONENTRENDERER_H
#define COCOENGINE_TILEMAPCOMPONENTRENDERER_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Memory/Ptrs.h"

namespace Coco
{
    class Entity;
    class RenderScene;
    class RenderContext;
    class Texture;

    class TileMapComponentRenderer
    {
    public:
        struct TilemapObjectData
        {
            Matrix4x4 Model;
            Vector4 Slice;
            Vector4 TintColor;
            SharedPtr<Texture> SpriteTexture;

            void SetDrawData(RenderContext& ctx) const;
        };

        static void Render(const Entity& tilemap, const Entity& camera, RenderScene& renderScene);
    };
} // Coco

#endif //COCOENGINE_TILEMAPCOMPONENTRENDERER_H