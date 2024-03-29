#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inUV;

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
    vec3 normal;
    vec4 tangent;
    vec2 uv;
    vec3 worldPosition;
} outVaryings;

void main() {
    outVaryings.normal = mat3(constants.model) * inNormal;
    outVaryings.tangent = vec4(mat3(constants.model) * inTangent.xyz, inTangent.w); 
    outVaryings.uv = inUV;
    outVaryings.worldPosition = (constants.model * vec4(inPosition, 1.0)).xyz;
    
    gl_Position = globalUBO.projection * globalUBO.view * vec4(outVaryings.worldPosition, 1.0);
}