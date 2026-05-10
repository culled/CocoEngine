struct CameraData
{
    float4x4 View;
    float4x4 Projection;
}
ParameterBlock<CameraData> cameraData;

struct ObjectData
{
    float4x4 Model;
    float4 TintColor;
}

[shader("vertex")]
float4 vsMain(float3 position: POSITION, uniform ObjectData objectData) : SV_Position {
    float4 world = mul(objectData.Model, float4(position, 1.0));
    float4 view = mul(cameraData.View, world);
    return mul(cameraData.Projection, view);
}

[shader("pixel")]
float4 psMain(float4 position: SV_Position, uniform ObjectData objectData) : SV_Target0 {
    return objectData.TintColor;
}
