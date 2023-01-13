#include "Common.hlsli"
#include "MyBRDF.hlsli"
#include "TangentHelper.hlsli"

Texture2D gAlbedo : register(t10);;
Texture2D gNormal : register(t11);;
Texture2D gMetallic : register(t12);;
Texture2D gRoughness : register(t13);;
Texture2D gAO : register(t14);;

TextureCube gIrradiance : register(t15);;
TextureCube gRadiance : register(t16);;
Texture2D gLUT : register(t17);;

SamplerState gsamLinerClamp : register(s0);
SamplerState gsamPointClamp : register(s1);

struct Attributes
{
    float3 positionL : POSITION;
    float3 normalL : NORMAL;
    float3 tangentL : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct Varyings
{
    float4 positionH : SV_POSITION;
    float3 positionW : TEXCOORD0;
    float3 normalW : NORMAL0;
    float3 tangentW : TANGENT0;
    float2 texcoord : TEXCOORD1;
};

Varyings vert(Attributes IN)
{
    Varyings OUT;
    float4 posW = mul(float4(IN.positionL, 1.0f), gWorld);
    OUT.positionH = mul(posW, gViewProj);
    OUT.positionW = posW.xyz;
    OUT.normalW = mul(IN.normalL, (float3x3) gWorld);
    OUT.tangentW = mul(IN.tangentL, (float3x3) gWorld);
    OUT.texcoord = IN.texcoord;
    return OUT;
}