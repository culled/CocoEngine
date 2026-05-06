struct CameraData
{
    float4x4 View;
    float4x4 Projection;
}
ParameterBlock<CameraData> cameraData;

struct ObjectData
{
    float4x4 Model;
}
ParameterBlock<ObjectData> objectData;

[shader("vertex")]
float4 vsMain(float3 position: POSITION) : SV_Position {
    float4 world = mul(objectData.Model, float4(position, 1.0));
    float4 view = mul(cameraData.View, world);
    float4 proj = mul(cameraData.Projection, view);
    return proj;
}

[shader("pixel")]
float4 psMain(float4 position: SV_Position) : SV_Target0 {
    return float4(0.8f, 0.4f, 0.2f, 1.0f);
}
