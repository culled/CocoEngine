#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec4 inTangent;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection2d;
} globalUBO;

layout(push_constant) uniform pushConstants {
    // Only guaranteed to be 128 bytes total
    mat4 model; // 64 bytes
} constants;

layout(location = 0) out vec2 outUV;

void main() {
    outUV = inUV;
    gl_Position = globalUBO.projection2d * constants.model * vec4(inPosition, 1.0);
}