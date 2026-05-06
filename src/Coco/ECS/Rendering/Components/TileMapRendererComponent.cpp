//
// Created by cullen on 5/4/26.
//

#include "TileMapRendererComponent.h"

namespace Coco
{
    TileMapRendererComponent::TileMapRendererComponent(const UUID& ownerID, SharedPtr<TileMap> map) :
        EntityComponent(ownerID),
        Map(map)
    {}
} // Coco