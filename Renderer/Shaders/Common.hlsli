cbuffer View : register(b0)
{
    float4x4 viewProjectionMatrix;
}

cbuffer RenderItem : register(b1)
{
    float4x4 worldMatrix;
}

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);

struct PixelIn
{
    float4 position : SV_Position;
    float2 uv: TEXCOORD;
    float3 normal: NORMAL;
    float3 tangent: TANGENT;
    float4 color: COLOR;
};

struct PixelOut
{
    float baseColor: SV_Target0;
};


float4 localToProjection(float3 position, float4x4 worldMatrix, float4x4 viewProjectionMatrix)
{
    return mul(mul(float4(position, 1.0f), worldMatrix), viewProjectionMatrix);
}