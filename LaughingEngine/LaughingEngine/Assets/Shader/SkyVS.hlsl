cbuffer VSConstants : register(b0)
{
    float4x4 gProjInverse;
    float4x4 gViewInverse;
};

struct VertextOut
{
    float4 PosH : SV_Position;
    float3 ViewDir : TEXCOORD;
};

VertextOut main(in uint id : SV_VERTEXID)
{
    float2 tex = float2(uint2(id, id << 1) & 2);
    float4 posH = float4(lerp(float2(-1, 1), float2(1, -1), tex), 1, 1);
    float4 posV = mul(posH, gProjInverse);
    
    VertextOut vout;
    vout.PosH = posH;
    vout.ViewDir = mul(posV.xyz / posV.w, (float3x3)gViewInverse);
    return vout;
}