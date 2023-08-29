#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct varyings
{
    vec2 uv;
    vec3 worldNormal;
    vec3 worldPosition;
    vec3 viewPosition;
    vec4 ambientColor;
    vec4 vertexColor;
    vec4 worldTangent;
} inVaryings;

layout(set = 1, binding = 0) uniform shaderUniformObject {
    vec4 baseColor;
    float shininess;
    float normalMapStrength;
} shaderUBO;

layout(set = 1, binding = 1) uniform sampler2D baseTexSampler;
layout(set = 1, binding = 2) uniform sampler2D roughnessTexSampler;
layout(set = 1, binding = 3) uniform sampler2D normalTexSampler;

layout(location = 0) out vec4 outColor;

struct directionalLight {
    vec3 direction;
    vec4 color;
};

directionalLight dirLight = {
    vec3(-0.57735, -0.57735, -0.57735),
    vec4(0.8, 0.8, 0.8, 1.0)
};

mat3 TBN;

vec4 calculateDirectionalLight(directionalLight light, float shininess, vec3 worldNormal, vec3 viewDirection);

void main() {
    vec3 worldNormal = normalize(inVaryings.worldNormal);
    vec3 worldTangent = normalize(inVaryings.worldTangent.xyz);
    worldTangent = (worldTangent - dot(worldTangent, worldNormal) * worldNormal);
    vec3 worldBitangent = cross(worldNormal, worldTangent) * inVaryings.worldTangent.w;
    TBN = mat3(worldTangent, worldBitangent, worldNormal);

    vec3 tangentSpaceNormal = 2.0 * texture(normalTexSampler, inVaryings.uv).rgb - 1.0;
    tangentSpaceNormal.g *= -1.0;

    vec3 bentWorldNormal = normalize(mix(worldNormal, TBN * tangentSpaceNormal, shaderUBO.normalMapStrength));

    vec3 viewDirection = normalize(inVaryings.viewPosition - inVaryings.worldPosition);

    float shininess = (1.0 - texture(roughnessTexSampler, inVaryings.uv).r) * shaderUBO.shininess;

    vec4 lightColor = inVaryings.ambientColor;
    lightColor += calculateDirectionalLight(dirLight, shininess, bentWorldNormal, viewDirection);

    vec4 albedo = shaderUBO.baseColor * texture(baseTexSampler, inVaryings.uv);

    outColor = vec4((albedo * lightColor).xyz, 1.0);
}

vec4 calculateDirectionalLight(directionalLight light, float shininess, vec3 worldNormal, vec3 viewDirection) {
    float NdotL = max(dot(worldNormal, -light.direction), 0.0);
    vec3 halfDir = normalize(viewDirection - light.direction);
    float specFactor = pow(max(dot(halfDir, worldNormal), 0.0), shininess);

    vec4 diffuse = light.color * NdotL;
    vec4 spec = light.color * specFactor * NdotL;

    return diffuse + spec;
}