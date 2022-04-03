// Draw(3)
// Build a big triangle
// texcoord:
// (0,0)   (1, 0)   (2,0)
//      -----------
//      |    |   /
//      |    | /
// (0,1)|----/ (1, 1)
//      |  /
//      |/
//      |(0,2)

#include "PresentRS.hlsli"

[RootSignature(Present_RootSig)]
void main(in uint id : SV_VERTEXID, out float4 posH : SV_POSITION, out float2 tex : TEXCOORD
)
{
    tex = float2(uint2(id, id << 1) & 2);
    posH = float4(lerp(float2(-1, 1), float2(1, -1), tex), 0, 1);
}