//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_VERSION_H
#define COCOENGINE_VERSION_H
#include "CoreTypes.h"
#include "String.h"

namespace Coco
{
    /// @brief Represents a version with a major, minor, and patch number
    struct Version
    {
        /// @brief The major number
        uint32 Major;

        /// @brief The minor number
        uint32 Minor;

        /// @brief The patch number
        uint32 Patch;
    };

    String ToString(const Version& version);
} // Coco

#endif //COCOENGINE_VERSION_H