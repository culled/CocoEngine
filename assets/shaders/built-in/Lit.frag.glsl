#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct varyings
{
    vec3 normal;
    vec4 tangent;
    vec2 uv;
    vec3 worldPosition;
} inVaryings;

layout(set = 0, binding = 0) uniform globalUniformObject {
    mat4 projection;
    mat4 view;
    vec3 viewPosition;
    vec4 ambientColor;
} globalUBO;

struct DirectionalLight {
    vec3 direction;
    vec4 color;
};

struct PointLight {
    vec3 position;
    vec4 color;
};

layout(set = 0, binding = 1) uniform LightBlock {
    int directionalLightCount;
    DirectionalLight directionalLights[2];
    int pointLightCount;
    PointLight pointLights[10];
} lightBO;

layout(set = 2, binding = 0) uniform shaderUniformObject {
    vec4 baseColor;
} shaderUBO;

layout(set = 2, binding = 1) uniform sampler2D baseTexSampler;
layout(set = 2, binding = 2) uniform sampler2D normalTexSampler;

layout(location = 0) out vec4 outColor;

vec4 calculateDirectionalLight(DirectionalLight light, float shininess, vec3 worldNormal, vec3 viewDirection);
vec4 calculatePointLight(PointLight light, float shininess, vec3 worldNormal, vec3 worldPosition, vec3 viewDirection);

void main() {
    vec3 worldNormal = normalize(inVaryings.normal);
    vec3 worldTangent = normalize(inVaryings.tangent.xyz);
    worldTangent = (worldTangent - dot(worldTangent, worldNormal) * worldNormal);
    vec3 worldBitangent = cross(worldNormal, worldTangent) * inVaryings.tangent.w;
    mat3 TBN = mat3(worldTangent, worldBitangent, worldNormal);

    vec3 viewDirection = normalize(globalUBO.viewPosition - inVaryings.worldPosition);

    vec4 albedo = shaderUBO.baseColor * texture(baseTexSampler, inVaryings.uv);
    vec4 light = globalUBO.ambientColor;

    vec3 tangentSpaceNormal = 2.0 * texture(normalTexSampler, inVaryings.uv).rgb - 1.0;
    tangentSpaceNormal.g *= -1.0;

    vec3 bentWorldNormal = normalize(mix(worldNormal, TBN * tangentSpaceNormal, 1.0));

    for(int i = 0; i < lightBO.directionalLightCount; i++)
    {
        vec4 dirLight = calculateDirectionalLight(lightBO.directionalLights[i], 32.0, bentWorldNormal, viewDirection);
        light += vec4(dirLight.rgb, 1.0) * dirLight.a;
    }

    for(int i = 0; i < lightBO.pointLightCount; i++)
    {
        vec4 pointLight = calculatePointLight(lightBO.pointLights[i], 32.0, bentWorldNormal, inVaryings.worldPosition, viewDirection);
        light += vec4(pointLight.rgb, 1.0) * pointLight.a;
    }

    outColor = albedo * vec4(light.xyz, 1.0);
}

vec4 calculateDirectionalLight(DirectionalLight light, float shininess, vec3 worldNormal, vec3 viewDirection) {
    float NdotL = max(-dot(worldNormal, light.direction), 0.0);

    vec3 diffuse = light.color.rgb * NdotL;

    vec3 halfDir = normalize(viewDirection - light.direction);
    float specFactor = pow(max(dot(halfDir, worldNormal), 0.0), shininess);
    vec3 spec = light.color.rgb * specFactor * NdotL;

    return vec4(diffuse + spec, light.color.a);
}

vec4 calculatePointLight(PointLight light, float shininess, vec3 worldNormal, vec3 worldPosition, vec3 viewDirection)
{
    vec3 posToLight = light.position - worldPosition;
    vec3 lightDir = normalize(posToLight);

    float NdotL = max(dot(worldNormal, lightDir), 0.0);

    float dist = length(posToLight);
    float attenuation = light.color.a / (dist * dist);

    vec3 diffuse = light.color.rgb * NdotL;

    vec3 halfDir = normalize(viewDirection + lightDir);
    float specFactor = pow(max(dot(halfDir, worldNormal), 0.0), shininess);
    vec3 spec = light.color.rgb * specFactor * NdotL;

    return vec4(diffuse + spec, attenuation);
}