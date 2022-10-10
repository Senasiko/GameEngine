cbuffer View : register(b0)
{
    float4x4 viewProjectionMatrix;
    uint2 viewSize;
}

cbuffer RenderItem : register(b1)
{
    float4x4 worldMatrix;
}

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);


float4 localToProjection(float3 position, float4x4 worldMatrix, float4x4 viewProjectionMatrix)
{
    return mul(mul(float4(position, 1.0f), worldMatrix), viewProjectionMatrix);
}