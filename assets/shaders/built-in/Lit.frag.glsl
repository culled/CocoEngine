#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct varyings
{
    vec3 normal;
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

layout(set = 0, binding = 1) uniform DirectionalLightBlock {
    int lightCount;
    DirectionalLight lights[2];
} directionalLights;

layout(set = 1, binding = 0) uniform shaderUniformObject {
    vec4 baseColor;
} shaderUBO;

layout(set = 1, binding = 1) uniform sampler2D baseTexSampler;

layout(location = 0) out vec4 outColor;

vec4 calculateDirectionalLight(DirectionalLight light, float shininess, vec3 worldNormal, vec3 viewDirection);

void main() {
    vec3 worldNormal = normalize(inVaryings.normal);
    vec3 viewDirection = normalize(globalUBO.viewPosition - inVaryings.worldPosition);

    vec4 albedo = shaderUBO.baseColor * texture(baseTexSampler, inVaryings.uv);
    vec4 light = globalUBO.ambientColor;

    for(int i = 0; i < directionalLights.lightCount; i++)
    {
        vec4 dirLight = calculateDirectionalLight(directionalLights.lights[i], 32.0, worldNormal, viewDirection);
        light += vec4(dirLight.rgb, 1.0) * dirLight.a;
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