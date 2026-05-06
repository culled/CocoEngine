struct CameraData
{
    float4x4 View;
    float4x4 Projection;
}
ParameterBlock<CameraData> cameraData;

struct ObjectData
{
    float4 TintColor;
    Sampler2D ColorTexture;
}

struct VertexData
{
    float4 position: SV_Position;
    float2 uv;
}

[shader("vertex")]
VertexData vsMain(float3 position: POSITION, float2 uv: TEXCOORD0) {
    float4 view = mul(cameraData.View, float4(position.xy, -1.0, 1.0));

    VertexData resultData;
    resultData.position = mul(cameraData.Projection, view);
    resultData.uv = uv;
    return resultData;
}

[shader("pixel")]
float4 psMain(VertexData vertexData, uniform ObjectData objectData) : SV_Target0 {
    float4 texColor = objectData.ColorTexture.Sample(vertexData.uv);
    return texColor * objectData.TintColor;
}
