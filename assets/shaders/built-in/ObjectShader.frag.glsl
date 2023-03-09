#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
}