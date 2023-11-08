#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection;
    mat4 view;
} globalUBO;

layout(push_constant) uniform pushConstants {
    // Only guaranteed to be 128 bytes total
    mat4 model; // 64 bytes
    int id; // 4 bytes
} constants;

layout(location = 0) out flat int outID;

void main() {
    gl_Position = globalUBO.projection * globalUBO.view * constants.model * vec4(inPosition, 1.0);

    outID = constants.id;
}