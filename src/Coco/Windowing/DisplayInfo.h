//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_DISPLAYINFO_H
#define COCOENGINE_DISPLAYINFO_H
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Math/Vector2.h>
#include <Coco/Core/Math/Size.h>

namespace Coco
{
    /// @brief Information for a display monitor
    struct DisplayInfo
    {
        /// @brief The name of the display
        String Name;

        /// @brief The position of the top-left corner of the display in virtual space
        Vector2i Offset;

        /// @brief The resolution of the display
        Sizei Resolution;

        /// @brief The DPI of the display
        uint16 DPI;

        /// @brief If true, this is the primary display for the platform
        bool IsPrimary;
    };
} // Coco

#endif //COCOENGINE_DISPLAYINFO_H