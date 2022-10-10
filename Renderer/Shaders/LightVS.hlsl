#include "Common.hlsli"
#include "LightCommon.hlsli"

struct VertexIn
{
    float3 position: POSITION;
};

LightPixelIn VSMain(VertexIn vertexIn)
{
    LightPixelIn result;
    result.position = localToProjection(vertexIn.position, worldMatrix, viewProjectionMatrix);
    return result;
}