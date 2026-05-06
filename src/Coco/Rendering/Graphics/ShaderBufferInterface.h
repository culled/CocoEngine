//
// Created by cullen on 3/21/26.
//

#ifndef COCOENGINE_SHADERBUFFERINTERFACE_H
#define COCOENGINE_SHADERBUFFERINTERFACE_H

#include <Coco/Core/Types/CoreTypes.h>

namespace slang
{
    class TypeLayoutReflection;
}

namespace Coco
{
    struct ShaderElementLocation;
    class Texture;

    class ShaderBufferInterface
    {
    public:
        virtual ~ShaderBufferInterface() = default;

        virtual void Write(const ShaderElementLocation& location, const void* data, uint64 dataSize) = 0;
        virtual void Write(const ShaderElementLocation& location, Texture* texture) = 0;
        virtual void Flush() = 0;

        slang::TypeLayoutReflection* GetTypeLayout() const { return _blockTypeLayout; }

    protected:
        slang::TypeLayoutReflection* _blockTypeLayout;

    protected:
        ShaderBufferInterface(slang::TypeLayoutReflection* blockTypeLayout);
    };
} // Coco

#endif //COCOENGINE_SHADERBUFFERINTERFACE_H