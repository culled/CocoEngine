#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
} globalUBO;

layout(push_constant) uniform pushConstants {
    // Only guaranteed to be 128 bytes total
    mat4 model; // 64 bytes
} constants;

layout(location = 0) out struct varyings
{
    vec2 uv;
    vec3 worldNormal;
    vec4 ambientColor;
} outVaryings;

void main() {
    outVaryings.uv = inUV;
    outVaryings.worldNormal = mat3(constants.model) * inNormal;
    outVaryings.ambientColor = globalUBO.ambientColor;

    gl_Position = globalUBO.projection * globalUBO.view * constants.model * vec4(inPosition, 1.0);
}