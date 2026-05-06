//
// Created by cullen on 3/10/26.
//

#include "Shader.h"

#include "RenderService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Shader, Resource);

    Shader::Shader(Engine* engine, uint64 id, const FilePath& shaderPath) :
        Resource(engine, id),
        _program()
    {
        RenderService* rendering = engine->GetService<RenderService>();
        COCO_ASSERT(rendering, "No active RenderService found");

        GraphicsPlatform* platform = rendering->GetGraphicsPlatform();
        COCO_ASSERT(platform, "GraphicsPlatform was not created");

        _program = platform->CreateShaderProgram(shaderPath);
    }

    Shader::~Shader()
    {
        if (_program)
        {
            if (RenderService* rendering = _engine->GetService<RenderService>())
            {
                if (GraphicsPlatform* platform = rendering->GetGraphicsPlatform())
                    platform->InvalidateResource(_program->GetID());
            }
        }
    }
} // Coco