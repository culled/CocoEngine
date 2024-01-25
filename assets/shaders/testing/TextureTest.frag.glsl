#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform globalUniformObject {
    float value;
} globalUBO;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, inUV);
    outColor = vec4(texColor.rgb, 1.0);
}