#include "Common.hlsli"

struct VertexIn
{
    float3 PosL : POSITION0;
    float4 Color : COLOR0;
};

struct VertextOut
{
    float4 PosH : SV_Position;
    float4 Color : COLOR0;
};

VertextOut main(VertexIn vin)
{
    VertextOut vout;
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    vout.Color = vin.Color;
    return vout;
}