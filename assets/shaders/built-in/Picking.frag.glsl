#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in flat int inID;

layout(location = 0) out int outID;

void main() {
    outID = inID;
}