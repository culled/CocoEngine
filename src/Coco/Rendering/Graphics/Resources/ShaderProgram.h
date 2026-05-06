//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_SHADERPROGRAM_H
#define COCOENGINE_SHADERPROGRAM_H
#include <slang.h>

#include "Coco/Rendering/Graphics/GraphicsResource.h"
#include "Coco/Rendering/Graphics/ShaderCursor.h"
#include "Coco/Rendering/ShaderTypes.h"

namespace slang
{
    class TypeLayoutReflection;
    typedef struct ShaderReflection ProgramLayout;
}

namespace Coco
{
    class ShaderProgram : public GraphicsResource
    {
    public:
        ~ShaderProgram() = default;

        virtual slang::ProgramLayout* GetProgramLayout() = 0;
        virtual int64 GetParamBlockIndex(const char* name) = 0;
        virtual slang::TypeLayoutReflection* GetParamBlockLayout(uint64 index) = 0;

    protected:
        ShaderProgram(uint64 id);
    };
} // Coco

#endif //COCOENGINE_SHADERPROGRAM_H