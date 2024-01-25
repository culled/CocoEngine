#version 450

layout(set = 0, binding = 0) uniform globalUniformObject {
    float value;
} globalUBO;

layout(set = 1, binding = 0) uniform shaderUniformObject {
    float value;
} shaderUBO;

layout(set = 2, binding = 0) uniform instanceUniformObject {
    float value;
} instanceUBO;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(globalUBO.value, shaderUBO.value, instanceUBO.value, 1.0);
}