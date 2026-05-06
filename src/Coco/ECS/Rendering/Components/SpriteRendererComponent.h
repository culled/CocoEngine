//
// Created by cullen on 4/5/26.
//

#ifndef COCOENGINE_SPRITERENDERERCOMPONENT_H
#define COCOENGINE_SPRITERENDERERCOMPONENT_H
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Types/Color.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    class Mesh;
    class Texture;

    struct SpriteRendererComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(SpriteRendererComponent)

    public:
        /// @brief The ID of the sprite mesh resource
        static uint64 SpriteMeshID;

        SharedPtr<Texture> SpriteTexture;
        Color TintColor;

        /// @brief The number of columns in the spritesheet
        uint32 Columns;

        /// @brief The number of rows in the spritesheet
        uint32 Rows;

        /// @brief The index of the current cell in the spritesheet
        uint32 AtlasCellIndex;

        /// @brief If true, the sprite will be mirrored on the X axis
        bool FlipX;

        /// @brief If true, the sprite will be mirrored on the Y axis
        bool FlipY;

        SpriteRendererComponent(const UUID& ownerEntityID);
        SpriteRendererComponent(const UUID& ownerEntityID, SharedPtr<Texture> spriteTexture, const Color& tintColor = Color::White);

        /// @brief Gets or creates the sprite mesh
        /// @return The sprite mesh
        static SharedPtr<Mesh> GetOrCreateSpriteMesh();

        /// @brief Sets the atlas of this sprite
        /// @param columns The number of columns
        /// @param rows The number of rows
        void SetAtlas(uint32 columns, uint32 rows) noexcept;

        /// @brief Gets the number of cells in the sprite atlas
        /// @return The number of cells
        uint32 GetAtlasCellCount() const noexcept { return Rows * Columns; }

        /// @brief Gets the slice of this sprite's current atlas cell
        /// @return The slice of the current cell in the format (offsetX, offsetY, sizeX, sizeY)
        Vector4 GetCurrentAtlasCellSlice() const noexcept;
    };
} // Coco

#endif //COCOENGINE_SPRITERENDERERCOMPONENT_H