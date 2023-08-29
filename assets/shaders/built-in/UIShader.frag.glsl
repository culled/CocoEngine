#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inUV;

layout(set = 1, binding = 0) uniform shaderUniformObject {
    vec4 baseColor;
} shaderUBO;

layout(set = 1, binding = 1) uniform sampler2D baseTexSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = shaderUBO.baseColor * texture(baseTexSampler, inUV);
}