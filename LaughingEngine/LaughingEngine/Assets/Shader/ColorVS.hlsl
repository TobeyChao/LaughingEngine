cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
}

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
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Color = vin.Color;
    return vout;
}