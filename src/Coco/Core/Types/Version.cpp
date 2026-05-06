//
// Created by cullen on 3/4/26.
//

#include "Version.h"

namespace Coco
{
    String ToString(const Version& version)
    {
        return FormatString("(%u.%u.%u)", version.Major, version.Minor, version.Patch);
    }
} // Coco