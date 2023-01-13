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
          
float4 frag(Varyings IN) : SV_Target
{
    // 采样反照率贴图
    float4 albedo = gAlbedo.Sample(gsamLinerClamp, IN.texcoord);
    // 采样法线贴图
    float3 normalSample = gNormal.Sample(gsamLinerClamp, IN.texcoord).rgb;
    // 采样金属度贴图
    float metalness = gMetallic.Sample(gsamLinerClamp, IN.texcoord).r;
    // 采样粗糙度贴图
    float roughness = gRoughness.Sample(gsamLinerClamp, IN.texcoord).r;
    // 采样环境光遮蔽贴图
    float ao = gAO.Sample(gsamLinerClamp, IN.texcoord).r;

    // 归一化法线、切线
    IN.normalW = normalize(IN.normalW);
    IN.tangentW = normalize(IN.tangentW);

    // 获取光源
    Light mainLight = gLights[0];
    // 光源颜色
    float3 lightColor = mainLight.Strength;
    // 法线
    float3 N = NormalSampleToWorldSpace(normalSample, IN.normalW, IN.tangentW);
    // 光方向
    float3 L = normalize(mainLight.Direction);
    // 视点方向
    float3 V = normalize(gEyePosW - IN.positionW);
    // 半程向量
    float3 H = normalize(L + V);

    float NdotL = max(0, dot(N, L));
    float NdotH = max(0, dot(N, H));
    float NdotV = max(0, dot(N, V));
    float LdotH = max(0, dot(H, L));
                
    float3 F0 = Fresnel0(albedo.rgb, metalness);

    float3 directLight = 0;
    {
        float3 Ks = FresnelSchlick(LdotH, F0);

        // 因为金属没有漫反射，会吸收折射光
        float3 Kd = lerp(float3(1, 1, 1) - Ks, float3(0, 0, 0), metalness);

        // float3 ambient = _GlossyEnvironmentColor * albedo.rgb * ao;
        float3 diffuse = Kd * albedo.rgb / PI;
        float3 spec = BRDF(NdotH, NdotV, NdotL, LdotH, roughness, albedo.rgb, metalness);
                    
        directLight = /*ambient + */(diffuse + spec) * lightColor * NdotL;
    }

    // IBL
    float3 ambientLight = 0;
    {
        float3 Ks = FresnelSchlickRoughness(NdotV, F0, roughness);
        float3 Kd = lerp(float3(1, 1, 1) - Ks, float3(0, 0, 0), metalness);;

        // 根据法线采样irradiance贴图
        float3 irradiance = gIrradiance.Sample(gsamLinerClamp, N).rgb;
        float3 diffuse = Kd * irradiance * albedo.rgb;

        // 根据视线反方向对法线反射的方位采样radiance贴图
        float3 R = reflect(-V, N);
        float lod = roughness * MAX_REFLECTION_LOD;
        float3 prefilteredColor = gRadiance.SampleLevel(gsamLinerClamp, R, lod).rgb;

        // 采样LUT BRDF积分贴图
        float2 samplePos = float2(NdotV, roughness);
        float2 envBRDF = gLUT.Sample(gsamLinerClamp, samplePos).rg;
        float3 specular = prefilteredColor * (Ks * envBRDF.x + envBRDF.y);

        ambientLight = (diffuse + specular)/* * ao*/;
    }

    return float4(directLight + ambientLight, 1.0f);
}