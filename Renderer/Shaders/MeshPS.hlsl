#include "MeshCommon.hlsli"

// Texture2D baseColorTexture: register(t0);

PixelOut PSMain(PixelIn input)
{
    PixelOut output;
    // output.baseColor = baseColorTexture.Sample(samplerWrap, input.uv);
    output.baseColor = input.color;
    return output;
}