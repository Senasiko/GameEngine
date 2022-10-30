cbuffer View : register(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionToWorldMatrix;
    float4 viewPosition;
    float viewNearZ;
    float viewFarZ;
    uint2 viewSize;
}

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);
SamplerState anisotropicWrap : register(s2);

float ProjectionZToViewZ(float depth, float nearZ, float farZ)
{
    return (2.0f * nearZ) / (farZ + nearZ - depth * (farZ - nearZ));
}

float ViewZToProjectionZ(float depth, float nearZ, float farZ)
{
    return (2.0f * nearZ / depth - farZ - nearZ) / (farZ - nearZ);
}

float4 ScreenToWorld(float4 screenPosition, float gBufferDepth, float2 screenSize, float4x4 projectionToWorldMatrix)
{
    float4 ws = mul(float4(screenPosition.xy / screenSize * 2 - 1, gBufferDepth, 1), projectionToWorldMatrix);
    return ws / ws.w;
}

float4 WorldToProjection(float4 worldPosition, float4x4 viewProjectionMatrix)
{
    return mul(worldPosition, viewProjectionMatrix);
}

float4 WorldToScreen(float4 worldPosition, float4x4 viewProjectionMatrix, float2 screenSize)
{
    float4 position = WorldToProjection(worldPosition, viewProjectionMatrix);
    return float4(screenSize.xy * (position.xy / position.w + 1) / 2, position.z / position.w, position.w);
}

float4 LocalToProjection(float3 position, float4x4 worldMatrix, float4x4 viewProjectionMatrix)
{
    return WorldToProjection(mul(float4(position, 1.0f), worldMatrix), viewProjectionMatrix);
}
