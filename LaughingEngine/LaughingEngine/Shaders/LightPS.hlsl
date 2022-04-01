#include "Common.hlsli"

struct VertextOut
{
    float4 PosH : SV_Position;
    float4 Color : COLOR;
};

float4 main(VertextOut pin) : SV_TARGET
{
    return pin.Color;
}