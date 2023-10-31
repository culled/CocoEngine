#version 450 core

// From ImGui vulkan backend
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aUV;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection;
} globalUBO;

out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;

    gl_Position = globalUBO.projection * vec4(aPos.xy, 0, 1);
}