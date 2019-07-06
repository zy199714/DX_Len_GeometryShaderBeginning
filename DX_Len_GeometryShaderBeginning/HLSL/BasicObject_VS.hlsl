#include "Basic.hlsli"

// ������ɫ��
VertexPosHWNormalTex VS(VertexPosNormalTex vIn)
{
    VertexPosHWNormalTex vOut;
    
    matrix viewProj = mul(gView, gProj);
    matrix lightViewProj = mul(gLightView, gLightProj);
    float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

    vOut.PosW = posW.xyz;
    vOut.PosH = mul(posW, viewProj);

    vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldInvTranspose);
    vOut.NormalW = normalize(vOut.NormalW);

    vOut.Tex = vIn.Tex;

    vOut.lightViewPositionH = mul(posW, lightViewProj);
    vOut.lightPos = normalize(gLightPosition.xyz - posW.xyz);
    return vOut;
}