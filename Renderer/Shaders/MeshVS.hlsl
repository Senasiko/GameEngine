#include "Common.hlsli"
#include "MeshCommon.hlsli"

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
    // result.position = float4(vertexIn.id % 2, vertexIn.id % 2, 0, 0);
    result.position = LocalToProjection(vertexIn.position, worldMatrix, viewProjectionMatrix);
    result.worldNormal = mul(vertexIn.normal, worldMatrix);
    result.worldPosition = mul(vertexIn.position, worldMatrix);
    result.uv = vertexIn.uv;
    result.tangent = vertexIn.tangent;
    result.color = vertexIn.color;
    return result;
}