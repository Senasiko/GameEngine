#include "Common.hlsli"
#include "MeshCommon.hlsli"

// Texture2D baseColorTexture: register(t0);

// struct PrePixelOut
// {
//     float depth: SV_Target0;
// };

void PSMain(PixelIn input)
{
    // PrePixelOut output;
    // // output.baseColor = baseColorTexture.Sample(samplerWrap, input.uv);
    // output.depth = ProjectionZToViewZ(input.position.z, viewNearZ, viewFarZ);
}