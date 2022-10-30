
struct LightPixelIn
{
    float4 position: SV_Position;
};

struct GBuffer
{
    float4 baseColor;
    float3 normal; 
    float depth;
};

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1

cbuffer LightConstant: register(b1)
{
    float4x4 worldMatrix;
    float4x4 shadowViewProjectionMatrix;
    float4 lightPosition;
    float2 shadowViewSize;
    uint lightType;
}

Texture2D<float4> GBufferBaseColorTexture: register(t0);
Texture2D<float4> GBufferNormalTexture: register(t1);
Texture2D<float4> GBufferDepthTexture: register(t2);
Texture2D<float> ShadowMap: register(t3);


GBuffer ResolveGBuffer(LightPixelIn pixelIn)
{
    float2 uv = float2(pixelIn.position.x / viewSize.x, pixelIn.position.y / viewSize.y);
    GBuffer gBuffer;
    gBuffer.baseColor = GBufferBaseColorTexture.Load(int3(pixelIn.position.xy, 0));
    gBuffer.normal = GBufferNormalTexture.Load(int3(pixelIn.position.xy, 0));
    gBuffer.depth = GBufferDepthTexture.Load(int3(pixelIn.position.xy, 0)).r;
    return gBuffer;
}

float ResolveShadow(float4 worldPosition)
{
    float4 positionInShadow = WorldToProjection(worldPosition, shadowViewProjectionMatrix);
    float shadow = ShadowMap.Sample(anisotropicWrap, (positionInShadow.xy / positionInShadow.w + 1) / 2);
    positionInShadow = float4(shadowViewSize.xy * (positionInShadow.xy / positionInShadow.w + 1) / 2, positionInShadow.z / positionInShadow.w, positionInShadow.w);
    float depth = positionInShadow.z;
    return depth > shadow ? 0 : 1;
}