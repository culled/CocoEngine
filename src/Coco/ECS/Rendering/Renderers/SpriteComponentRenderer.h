//
// Created by cullen on 5/11/26.
//

#ifndef COCOENGINE_SPRITECOMPONENTRENDERER_H
#define COCOENGINE_SPRITECOMPONENTRENDERER_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Memory/Ptrs.h"

namespace Coco
{
    class Entity;
    class RenderContext;
    class RenderScene;
    class Texture;

    class SpriteComponentRenderer
    {
    public:
        struct SpriteObjectData
        {
            Matrix4x4 Model;
            Vector4 Slice;
            Vector4 TintColor;
            SharedPtr<Texture> SpriteTexture;

            void SetDrawData(RenderContext& ctx) const;
        };

        static void Render2D(const Entity& sprite, RenderScene& renderScene);
        static void Render3D(const Entity& sprite, const Vector3& cameraPosition, RenderScene& renderScene);
    };
} // Coco

#endif //COCOENGINE_SPRITECOMPONENTRENDERER_H