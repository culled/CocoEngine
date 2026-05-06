//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_SHADER_H
#define COCOENGINE_SHADER_H
#include "ShaderTypes.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Resources/Resource.h"
#include "Graphics/Resources/ShaderProgram.h"

namespace Coco
{
    class GraphicsPlatform;

    class Shader : public Resource
    {
        DECLARE_RTTI_TYPE(Shader);

    public:
        Shader(Engine* engine, uint64 id, const FilePath& shaderPath);
        ~Shader();

        Ref<ShaderProgram> GetProgram() { return _program; }

    private:
        Ref<ShaderProgram> _program;
    };
} // Coco

#endif //COCOENGINE_SHADER_H