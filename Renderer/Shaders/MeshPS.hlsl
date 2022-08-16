#include "Common.hlsli"

Texture2D baseColorTexture: register(t0);

float4 PSMain(PixelIn input): SV_Target0
{
    float4 color = baseColorTexture.Sample(samplerWrap, input.uv);
    return color;
}