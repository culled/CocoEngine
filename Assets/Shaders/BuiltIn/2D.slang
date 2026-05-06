struct CameraData2D
{
    float4x4 Projection;
}
ParameterBlock<CameraData2D> cameraData2D;

struct DrawData
{
    Sampler2D ColorTexture;
}

struct VertexData
{
    float4 position: SV_Position;
    float4 color;
    float2 uv;
}

[shader("vertex")]
VertexData vsMain(float3 position: POSITION, float4 color: COLOR, float2 uv: TEXCOORD0) {
    VertexData resultData;
    resultData.position = mul(cameraData2D.Projection, float4(position.xy, 0.0, 1.0));
    resultData.color = color;
    resultData.uv = uv;
    return resultData;
}

[shader("pixel")]
float4 psMain(VertexData vertexData, uniform DrawData drawData) : SV_Target0 {
    float4 texColor = drawData.ColorTexture.Sample(vertexData.uv);
    return texColor * vertexData.color;
}
