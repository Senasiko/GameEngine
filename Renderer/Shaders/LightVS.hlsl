#include "Common.hlsli"
#include "LightCommon.hlsli"

struct VertexIn
{
    float3 position: POSITION;
};

LightPixelIn VSMain(VertexIn vertexIn)
{
    LightPixelIn result;
    [BRANCH]
    if (lightType == LIGHT_TYPE_DIRECTIONAL)
    {
        result.position = float4(vertexIn.position.xy / abs(vertexIn.position).xy, 0, 1);
    } else
    {
        result.position = LocalToProjection(vertexIn.position, worldMatrix, viewProjectionMatrix);
    }
    return result;
}