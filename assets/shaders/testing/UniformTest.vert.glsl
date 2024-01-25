#version 450

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform pushConstants {
    // Only guaranteed to be 128 bytes total
    vec3 offset; // 12 bytes
} constants;

void main() {
    gl_Position = vec4(inPosition + constants.offset, 1.0);
}