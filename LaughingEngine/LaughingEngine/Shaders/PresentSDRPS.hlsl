#include "ColorSpaceUtility.hlsli"
#include "PresentRS.hlsli"

Texture2D<float3> ColorTex : register(t0);

//https://zhuanlan.zhihu.com/p/455189480
//在PixelShader中，SV_POSITION的 xy 坐标表示屏幕中的像素坐标加上0.5的位置偏移

[RootSignature(Present_RootSig)]
float3 main(float4 posH : SV_POSITION) : SV_TARGET
{
    float3 linearRGB = ColorTex[(int2)posH.xy];
    return ApplyDisplayProfile(linearRGB, COLOR_FORMAT_sRGB_FULL);
}