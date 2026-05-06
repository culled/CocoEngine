//
// Created by cullen on 3/21/26.
//

#include "ShaderBufferInterface.h"

namespace Coco
{
    ShaderBufferInterface::ShaderBufferInterface(slang::TypeLayoutReflection* blockTypeLayout) :
        _blockTypeLayout(blockTypeLayout)
    {}
} // Coco