#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct varyings
{
    vec2 uv;
    vec3 worldNormal;
    vec4 ambientColor;
} inVaryings;

struct directionalLight {
    vec3 direction;
    vec4 color;
};

directionalLight dirLight = {
    vec3(-0.57735, -0.57735, -0.57735),
    vec4(0.8, 0.8, 0.8, 1.0)
};

layout(set = 1, binding = 0) uniform shaderUniformObject {
    vec4 baseColor;
} shaderUBO;

layout(set = 1, binding = 1) uniform sampler2D baseTexSampler;

layout(location = 0) out vec4 outColor;

vec4 calculateDirectionalLight(directionalLight light, vec3 worldNormal);

void main() {
    vec3 worldNormal = normalize(inVaryings.worldNormal);

    vec4 lightColor = inVaryings.ambientColor;
    lightColor += calculateDirectionalLight(dirLight, worldNormal);

    vec4 albedo = shaderUBO.baseColor * texture(baseTexSampler, inVaryings.uv);

    outColor = vec4((albedo * lightColor).xyz, 1.0);
}

vec4 calculateDirectionalLight(directionalLight light, vec3 worldNormal) {
    float NdotL = max(dot(worldNormal, -light.direction), 0.0);

    return light.color * NdotL;
}