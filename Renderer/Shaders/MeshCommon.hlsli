struct PixelIn
{
    float4 position : SV_Position;
    float2 uv: TEXCOORD;
    float3 normal: NORMAL;
    float3 tangent: TANGENT;
    float4 color: COLOR;
};

struct PixelOut
{
    float4 baseColor: SV_Target0;
};
