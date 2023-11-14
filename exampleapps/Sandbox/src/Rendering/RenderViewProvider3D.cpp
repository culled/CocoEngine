#include "RenderViewProvider3D.h"

#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>

const GlobalShaderUniformLayout RenderViewProvider3D::sGlobalLayout = GlobalShaderUniformLayout(
    {
        ShaderUniform("ProjectionMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity),
        ShaderUniform("ViewMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity),
        ShaderUniform("ViewPosition", ShaderUniformType::Float3, ShaderStageFlags::Vertex | ShaderStageFlags::Fragment, Vector3::Zero),
        ShaderUniform("AmbientColor", ShaderUniformType::Color, ShaderStageFlags::Vertex | ShaderStageFlags::Fragment, Color::Black)
    },
    {
        //ShaderBufferUniform("Lights", ShaderStageFlags::Fragment, RenderPass3D::sLightingBufferLayout.GetTotalSize(RenderService::cGet()->GetDevice()))
    }
);