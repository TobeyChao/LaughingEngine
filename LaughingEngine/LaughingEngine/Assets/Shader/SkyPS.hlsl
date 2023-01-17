cbuffer PSConstants : register(b1)
{
    float gTextureLevel;
};

struct VertextOut
{
    float4 PosH : SV_Position;
    float3 ViewDir : TEXCOORD;
};

TextureCube<float3> gSky : register(t10);

SamplerState gsamLinerWrap : register(s0);
SamplerState gsamPointWrap : register(s1);

float4 main(VertextOut pin) : SV_TARGET
{
    return float4(gSky.SampleLevel(gsamLinerWrap, pin.ViewDir, gTextureLevel), 1.0f);
}