#include "Common.hlsli"

Texture2D gDiffuseMap : register(t10);

SamplerState gsamLinerWrap : register(s0);
SamplerState gsamPointWrap : register(s1);

struct PixelOut
{
    float4 PosH : SV_Position;
    float3 PosW : POSITION0;
    float3 NormalW : NORMAL0;
    float3 Color : COLOR0;
    float2 TexC : TEXCOORD;
};

float4 main(PixelOut pin) : SV_TARGET
{
    Light mainLight = gLights[0];
    float3 lightColor = mainLight.Strength;
    float3 N = normalize(pin.NormalW);
    float3 L = normalize(-mainLight.Direction);
    float3 V = normalize(gEyePosW - pin.PosW);
    float3 H = normalize(L + V);
    float NdotL = max(0, dot(N, L));
    float NdotH = max(0, dot(N, H));
    float NdotV = max(0, dot(N, V));
    float LdotH = max(0, dot(H, L));
    
    float3 albedo = gDiffuseMap.Sample(gsamLinerWrap, pin.TexC).rgb;
    float3 diffuse = albedo * gLights[0].Strength * NdotL;
    float3 ambientColor = gAmbientLight.rgb * albedo;
    float spec = pow(NdotH, 32);
    float specularStrength = 10;
    float3 specular = specularStrength * spec * lightColor * albedo;
    return float4(ambientColor + diffuse + specular, 1.0f);
}