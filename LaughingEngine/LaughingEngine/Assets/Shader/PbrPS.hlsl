#include "Common.hlsli"
#include "MyBRDF.hlsli"
#include "TangentHelper.hlsli"

Texture2D gAlbedo : register(t0);;
Texture2D gNormal : register(t1);;
Texture2D gMetallic : register(t2);;
Texture2D gRoughness : register(t3);;
Texture2D gAO : register(t4);;

TextureCube gIrradiance;
TextureCube gRadiance;
Texture2D gLUT;

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
    // ������������ͼ
    float4 albedo = gAlbedo.Sample(gsamLinerClamp, IN.texcoord);
    // ����������ͼ
    float3 normalSample = gNormal.Sample(gsamLinerClamp, IN.texcoord).rgb;
    // ������������ͼ
    float metalness = gMetallic.Sample(gsamLinerClamp, IN.texcoord).r;
    // �����ֲڶ���ͼ
    float roughness = gRoughness.Sample(gsamLinerClamp, IN.texcoord).r;
    // �����������ڱ���ͼ
    float ao = gAO.Sample(gsamLinerClamp, IN.texcoord).r;

    // ��һ�����ߡ�����
    IN.normalW = normalize(IN.normalW);
    IN.tangentW = normalize(IN.tangentW);

    // ��ȡ��Դ
    Light mainLight = gLights[0];
    // ��Դ��ɫ
    float3 lightColor = mainLight.Strength;
    // ����
    float3 N = NormalSampleToWorldSpace(normalSample, IN.normalW, IN.tangentW);
    // �ⷽ��
    float3 L = normalize(mainLight.Direction);
    // �ӵ㷽��
    float3 V = normalize(gEyePosW - IN.positionW);
    // �������
    float3 H = normalize(L + V);

    float NdotL = max(0, dot(N, L));
    float NdotH = max(0, dot(N, H));
    float NdotV = max(0, dot(N, V));
    float LdotH = max(0, dot(H, L));
                
    float3 F0 = Fresnel0(albedo.rgb, metalness);

    float3 directLight = 0;
    {
        float3 Ks = FresnelSchlick(LdotH, F0);

        // ��Ϊ����û�������䣬�����������
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

        // ���ݷ��߲���irradiance��ͼ
        float3 irradiance = gIrradiance.Sample(gsamLinerClamp, N).rgb;
        float3 diffuse = Kd * irradiance * albedo.rgb;

        // �������߷�����Է��߷���ķ�λ����radiance��ͼ
        float3 R = reflect(-V, N);
        float lod = roughness * MAX_REFLECTION_LOD;
        float3 prefilteredColor = gRadiance.SampleLevel(gsamLinerClamp, R, lod).rgb;

        // ����LUT BRDF������ͼ
        float2 samplePos = float2(NdotV, roughness);
        float2 envBRDF = gLUT.Sample(gsamLinerClamp, samplePos).rg;
        float3 specular = prefilteredColor * (Ks * envBRDF.x + envBRDF.y);

        ambientLight = (diffuse + specular) * ao;
    }

    return float4(directLight + ambientLight, 1.0f);
}