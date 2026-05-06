struct CameraData
{
    float4x4 View;
    float4x4 Projection;
}
ParameterBlock<CameraData> cameraData;

struct MaterialData
{
    float4 TintColor;
    Sampler2D ColorTexture;
}
ParameterBlock<MaterialData> materialData;

struct ObjectData
{
    float4x4 Model;
}

struct VertexData
{
    float4 position: SV_Position;
    float2 uv;
}

[shader("vertex")]
VertexData vsMain(float3 position: POSITION, float2 uv: TEXCOORD0, uniform ObjectData objectData) {
    float4 world = mul(objectData.Model, float4(position, 1.0));
    float4 view = mul(cameraData.View, world);

    VertexData resultData;
    resultData.position = mul(cameraData.Projection, view);
    resultData.uv = uv;
    return resultData;
}

[shader("pixel")]
float4 psMain(VertexData vertexData) : SV_Target0 {
    float4 texColor = materialData.ColorTexture.Sample(vertexData.uv);
    return texColor * materialData.TintColor;
}
