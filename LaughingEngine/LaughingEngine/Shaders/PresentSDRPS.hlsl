#include "ColorSpaceUtility.hlsli"
#include "PresentRS.hlsli"

Texture2D<float3> ColorTex : register(t0);

//https://zhuanlan.zhihu.com/p/455189480
//��PixelShader�У�SV_POSITION�� xy �����ʾ��Ļ�е������������0.5��λ��ƫ��

[RootSignature(Present_RootSig)]
float3 main(float4 posH : SV_POSITION) : SV_TARGET
{
    float3 linearRGB = ColorTex[(int2)posH.xy];
    return ApplyDisplayProfile(linearRGB, COLOR_FORMAT_sRGB_FULL);
}