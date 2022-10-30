#include "Common.hlsli"
#include "LightCommon.hlsli"
#include "BRDF.hlsli"

float4 PSMain(LightPixelIn pixelIn): SV_Target0
{
    GBuffer gBuffer = ResolveGBuffer(pixelIn);
    float depth = gBuffer.depth; 
    float4 worldPosition = ScreenToWorld(pixelIn.position, gBuffer.depth, viewSize, projectionToWorldMatrix);
    float4 newP = WorldToScreen(worldPosition, viewProjectionMatrix, viewSize);
    float4 positionInShadow = WorldToProjection(worldPosition, shadowViewProjectionMatrix);
    positionInShadow = float4(shadowViewSize.xy * (positionInShadow.xy / positionInShadow.w + 1) / 2, positionInShadow.z / positionInShadow.w, positionInShadow.w);

    if (positionInShadow.x < 0 || positionInShadow.y < 0)
    {
        return gBuffer.baseColor;
    }

    float3 color = GGXBRDF(lightPosition, viewPosition - worldPosition, gBuffer.normal * 2 - 1, gBuffer.baseColor, float3(1.f, 1.f, 1.f), 0);
    return float4(color, 1.0f);
}