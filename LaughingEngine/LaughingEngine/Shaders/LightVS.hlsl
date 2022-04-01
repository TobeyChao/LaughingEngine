#include "Common.hlsli"

struct VertexIn
{
    float3 PosL : POSITION0;
    float2 Texcoord : TEXCOORD;
    float3 NormalL : NORMAL;
    float4 Color : COLOR;
};

struct VertextOut
{
    float4 PosH : SV_Position;
    float3 PosW : POSITION0;
    float3 NormalW : NORMAL0;
    float3 Color : COLOR0;
    float2 TexC : TEXCOORD;
};

VertextOut main(VertexIn vin)
{
    VertextOut vout;
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
    vout.PosH = mul(posW, gViewProj);
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vout.Color = vin.Color.rgb;
    vout.TexC = vin.Texcoord;
    return vout;
}