#ifndef __LIGHTING_UTIL_HLSLI__
#define __LIGHTING_UTIL_HLSLI__

#define MaxLights 16

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};

struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction; // directional/spot light only
    float FalloffEnd; // point/spot light only
    float3 Position; // point light only
    float SpotPower; // spot light only
};

float3 SchlickFresnel(float3 R0, float3 normal, float3 worldLightDir)
{
    float cosIncidentAngle = saturate(dot(worldLightDir, normal));
    // 光的方向和法线的方向越接近90度菲涅尔现象就越明显
    float f0 = 1.0f - cosIncidentAngle;
    float3 fresnelFactor = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);
    return fresnelFactor;
}

float3 BlinnPhong(float3 lightStrength, float3 worldLightDir, float3 worldNormal, float3 toEye, Material mat)
{
    float3 halfDir = normalize(toEye + worldLightDir);
    float m = mat.Shininess * 256.0f;
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfDir, worldNormal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfDir, worldLightDir);
    float3 specAlbedo = fresnelFactor * roughnessFactor;
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 ComputeDirectionalLight(Light light, Material mat, float3 worldNormal, float3 toEye)
{
    // 灯光方向
    float3 worldLightDir = normalize(-light.Direction);
    // 朗伯余弦定律
    float cosIncidentAngle = saturate(dot(worldLightDir, worldNormal));
    // 光强
    float3 lightStrength = light.Strength.rgb * cosIncidentAngle;
    // 计算BlinnPhong
    return BlinnPhong(lightStrength, worldLightDir, worldNormal, toEye, mat);
}

float4 ComputeLighting(Light gLights[MaxLights],
Material mat,
float3 pos, float3 worldNormal, float3 toEye,
float3 shadowFactor)
{
    float3 result = 0.0f;
    int i = 0;
#if (NUM_DIR_LIGHTS > 0)
        [unroll]
        for (i = 0; i < NUM_DIR_LIGHTS; ++i)
        {
            result += shadowFactor[0] * ComputeDirectionalLight(gLights[i], mat, worldNormal, toEye);
        }
#endif
    return float4(result, 0.0f);
}
#endif //__LIGHTING_UTIL_HLSLI__