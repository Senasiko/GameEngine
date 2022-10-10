#include "Common.hlsli"
#include "LightCommon.hlsli"

Texture2D<float4> GBufferBaseColor: register(t0);
Texture2D<float4> GBufferDepth: register(t1);


float4 PSMain(LightPixelIn pixelIn): SV_Target0
{
    return GBufferBaseColor.Sample(samplerWrap, float2(pixelIn.position.x / viewSize.x, pixelIn.position.y / viewSize.y));
}