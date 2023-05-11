#define MAX_REFLECTION_LOD 10
#define PI 3.1415926

// 法线分布函数
float TrowbridgeReitzGGX(float3 N, float3 H, float roughness)
{
    float roughness2 = roughness * roughness;
    float roughness4 = roughness2 * roughness2;
    float NdotH = max(0, dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float nom = roughness4;
    float denom = NdotH2 * (roughness4 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return nom / denom;
}

float TrowbridgeReitzGGX(float NdotH, float roughness)
{
    float roughness2 = roughness * roughness;
    float roughness4 = roughness2 * roughness2;
    float NdotH2 = NdotH * NdotH;

    float nom = roughness4;
    float denom = NdotH2 * (roughness4 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return nom / denom;
}

// 计算菲涅尔项的F0
float3 Fresnel0(float3 color, float metalness)
{
    float3 F0 = 0.04f;
    F0 = lerp(F0, color, metalness);
    return F0;
}

// 菲涅尔项
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float smoothness = 1.0 - roughness;
    return F0 + (max(float3(smoothness, smoothness, smoothness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlick(float cosTheta, float3 color, float metalness)
{
    float3 F0 = Fresnel0(color, metalness);
    return FresnelSchlick(cosTheta, F0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 color, float metalness, float roughness)
{
    float3 F0 = Fresnel0(color, metalness);
    return FresnelSchlickRoughness(cosTheta, F0, roughness);
}

// 几何函数
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

float GeometrySmith(float NdotV, float NdotL, float k)
{
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

float GeometrySmithIBL(float3 N, float3 V, float3 L, float roughness)
{
    float k = roughness * roughness;
    k = k / 2.0f;
    return GeometrySmith(N, V, L, k);
}

float GeometrySmithDirect(float3 N, float3 V, float3 L, float roughness)
{
    float k = roughness + 1.0f;
    k = k * k;
    k = k / 8.0f;
    return GeometrySmith(N, V, L, k);
}

float GeometrySmithIBL(float NdotV, float NdotL, float roughness)
{
    float k = roughness * roughness;
    k = k / 2.0f;
    return GeometrySmith(NdotV, NdotL, k);
}

float GeometrySmithDirect(float NdotV, float NdotL, float roughness)
{
    float k = roughness + 1.0f;
    k = k * k;
    k = k / 8.0f;
    return GeometrySmith(NdotV, NdotL, k);
}

float3 BRDF(float NdotH, float NdotV, float NdotL, float LdotH, float roughness, float3 color, float metalness)
{
    float D = TrowbridgeReitzGGX(NdotH, roughness);
    float3 F = FresnelSchlick(LdotH, color, metalness);
    float G = GeometrySmithDirect(NdotV, NdotL, roughness);

    float3 nominator = F * D * G;
    float denominator = 4.0 * NdotV * NdotL + 0.001f;
    float3 specular = nominator / denominator;
    return specular;
}