//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_VERSION_H
#define COCOENGINE_VERSION_H
#include "CoreTypes.h"
#include "String.h"

namespace Coco
{
    struct Version
    {
        uint32 Major;
        uint32 Minor;
        uint32 Patch;
    };

    String ToString(const Version& version);
} // Coco

#endif //COCOENGINE_VERSION_H