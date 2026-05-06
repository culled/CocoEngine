//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_OPENGLSHADERPROGRAM_H
#define COCOENGINE_OPENGLSHADERPROGRAM_H
#include "Coco/Core/Types/StackArray.h"
#include "Coco/Rendering/Graphics/Resources/ShaderProgram.h"
#include "../../../ShaderTypes.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Rendering/Graphics/VertexDataTypes.h"
#include <slang.h>
#include <slang-com-ptr.h>

namespace Coco
{
    class OpenGLGraphicsPlatform;;

    class OpenGLShaderProgram : public ShaderProgram
    {
    public:
        OpenGLShaderProgram(OpenGLGraphicsPlatform* platform, uint64 id, const FilePath& shaderPath);
        ~OpenGLShaderProgram();

        slang::TypeLayoutReflection* GetParamBlockLayout(const char* name) override;

        void Bind();

    private:
        OpenGLGraphicsPlatform* _platform;
        uint32 _programID;
        bool _isReady;
        Array<VertexChannel> _vertexChannels;

        Slang::ComPtr<slang::IComponentType> _linkedProgram;
        slang::TypeLayoutReflection* _typeLayoutInfo;

    private:
        uint32 CreateShader(ShaderStageType stage, uint8 entryPointIndex);
        void CreateProgram();
    };
} // Coco

#endif //COCOENGINE_OPENGSHADERPROGRAM_H