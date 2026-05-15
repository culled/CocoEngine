//
// Created by cullen on 5/4/26.
//

#include "TileMapRendererComponent.h"

namespace Coco
{
    TileMapRendererComponent::TileMapRendererComponent(const UUID& ownerID, SharedPtr<TileMap> map) :
        EntityComponent(ownerID),
        Map(map),
        DefaultTileID(std::numeric_limits<uint32>::max())
    {}

    void TileMapRendererComponent::CallForVisibleTiles(const Rect& localViewport,
        const std::function<void(const TileMapCell&)>& callbackFunction) const
    {
        Vector2i startingCell(static_cast<int>(Math::Floor(localViewport.Offset.X())), static_cast<int>(Math::Floor(localViewport.Offset.Y())));
        Vector2i cellCount(
            static_cast<int>(Math::Ceil(localViewport.Size.Width)) + 1,
            static_cast<int>(Math::Ceil(localViewport.Size.Height)) + 1
        );

        for (int x = 0; x < cellCount.X(); x++)
        {
            for (int y = 0; y < cellCount.Y(); y++)
            {
                Vector2i cellCoord(x + startingCell.X(), y + startingCell.Y());
                auto cellData = Map->GetCell(cellCoord);
                if (cellData)
                {
                    callbackFunction(*cellData);
                    continue;
                }

                if (DefaultTileID == std::numeric_limits<uint32>::max())
                        continue;

                TileMapCell tempData(cellCoord, DefaultTileID);
                callbackFunction(tempData);
            }
        }
    }
} // Coco