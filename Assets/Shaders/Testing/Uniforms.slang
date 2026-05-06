struct GlobalSceneData
{
    float seconds;
}
ParameterBlock<GlobalSceneData> globalSceneData;

struct ObjectData
{
    float2 offset;
}
ParameterBlock<ObjectData> objectData;

[shader("vertex")]
float4 vsMain(float3 position: POSITION) : SV_Position {
    return float4(position.xy + objectData.offset, position.z, 1.0f);
}

[shader("pixel")]
float4 psMain(float4 position: SV_Position) : SV_Target0 {
    float v = cos(globalSceneData.seconds) * 0.5f + 0.5f;
    return float4(v, 0.4f, 0.2f, 1.0f);
}
