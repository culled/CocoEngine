#pragma once
#include "../Graphics/RenderView.h"
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
    /// @brief Extra object data for a debug object
    struct DebugRenderObjectExtraData :
        public ExtraObjectData
    {
        /// @brief The color for the object
        Color ObjectColor;

        DebugRenderObjectExtraData(const Color& color);
    };
}