//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_WINDOWTYPES_H
#define COCOENGINE_WINDOWTYPES_H
#include <Coco/Core/Types/CoreTypes.h>
#include "Coco/Core/Types/EnumTypes.h"

namespace Coco
{
    /// @brief States that a Window can be in
    enum class WindowState : uint8
    {
        /// @brief The default state
        Default = 0,

        /// @brief The Window is minimized
        Minimized,

        /// @brief The Window is maximized
        Maximized
    };

    /// @brief Gets the string representation of a WindowState
    /// @param state The window state
    /// @return The string representation
    const char* ToString(WindowState state);

    /// @brief Modes for a confining a cursor to a Window
    enum class CursorConfineMode : uint8
    {
        /// @brief The cursor is not confined
        None = 0,

        /// @brief The cursor cannot move outside the Window's client area
        ClientArea,

        /// @brief The cursor is locked to the Window's center
        LockedCenter,

        /// @brief The cursor is locked where it currently is
        LockedInPlace,
    };

    /// @brief Style flags for a Window
    enum class WindowStyleFlags : uint8
    {
        /// @brief No style flags
        None = 0,

        /// @brief The Window will not include any decorations (title bar, buttons, frame, etc.)
        NoDecoration = 1 << 0,

        /// @brief This Window will not show up in the toolbar of desktop window managers
        NoTaskbarIcon = 1 << 1,

        /// @brief The Window will be placed above all other non-topmost windows and stay above them
        TopMost = 1 << 2
    };

    EnumFlagOperators(WindowStyleFlags)
}

#endif //COCOENGINE_WINDOWTYPES_H