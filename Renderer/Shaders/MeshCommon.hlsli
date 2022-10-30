struct PixelIn
{
    float4 position : SV_Position;
    float4 color: COLOR;
    float3 worldPosition: WORLD_POSITION;
    float3 worldNormal: WORLD_NORMAL;
    float3 tangent: TANGENT;
    float2 uv: TEXCOORD;
};

struct PixelOut
{
    float4 baseColor: SV_Target0;
    float3 normal: SV_Target1;
};


cbuffer MeshConstant : register(b1)
{
    float4x4 worldMatrix;
}
