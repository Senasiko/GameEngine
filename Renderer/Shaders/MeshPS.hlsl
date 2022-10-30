#include "Common.hlsli"
#include "MeshCommon.hlsli"

// Texture2D baseColorTexture: register(t0);

PixelOut PSMain(PixelIn input)
{
    PixelOut output;
    // output.depth = ProjectionZToViewZ(input.position.z, viewNearZ, viewFarZ)
    output.baseColor = input.color;
    output.normal = (normalize(input.worldNormal) + 1) / 2;
    return output;
}