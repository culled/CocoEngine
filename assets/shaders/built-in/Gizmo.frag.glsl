#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct varyings
{
    vec4 color;
} inVaryings;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inVaryings.color.xyz, 1.0);
}