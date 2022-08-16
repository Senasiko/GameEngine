#include "Common.hlsli"
struct VertexIn
{
    float3 position: POSITION;
    float3 normal: NORMAL;
    float3 tangent: TANGENT;
    float4 color: COLOR;
    float2 uv: TEXCOORD;
    uint id: SV_VertexId;
};

PixelIn VSMain(VertexIn vertexIn)
{
    PixelIn result;
    result.position = localToProjection(vertexIn.position, worldMatrix, viewProjectionMatrix);
    result.normal = vertexIn.normal;
    result.uv = vertexIn.uv;
    result.tangent = vertexIn.tangent;
    result.color = vertexIn.color;
    return result;
}