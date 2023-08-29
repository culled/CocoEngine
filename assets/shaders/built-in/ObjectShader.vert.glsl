#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec4 inTangent;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection;
    mat4 view;
    vec3 viewPosition;
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
    vec3 worldPosition;
    vec3 viewPosition;
    vec4 ambientColor;
    vec4 vertexColor;
    vec4 worldTangent;
} outVaryings;

void main() {
    outVaryings.uv = inUV;
    outVaryings.worldNormal = mat3(constants.model) * inNormal;
    outVaryings.worldPosition = (constants.model * vec4(inPosition, 1.0)).xyz;
    outVaryings.viewPosition = globalUBO.viewPosition;
    outVaryings.ambientColor = globalUBO.ambientColor;
    outVaryings.vertexColor = inColor;
    outVaryings.worldTangent = vec4(normalize(mat3(constants.model) * inTangent.xyz), inTangent.w);

    gl_Position = globalUBO.projection * globalUBO.view * vec4(outVaryings.worldPosition, 1.0);
}