[shader("vertex")]
float4 vsMain(float3 position: POSITION) : SV_Position {
    return float4(position, 1.0f);
}

[shader("pixel")]
float4 psMain(float4 position: SV_Position) : SV_Target0 {
    return float4(1.0f, 0.5f, 0.2f, 1.0f);
}