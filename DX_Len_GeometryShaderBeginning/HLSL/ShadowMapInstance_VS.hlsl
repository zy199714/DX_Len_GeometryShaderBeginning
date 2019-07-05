#include "Basic.hlsli"

SHADOW_PS_INPUT VS(InstancePosNormalTex vIn)
{
    SHADOW_PS_INPUT vout;
    vout.position = mul(float4(vIn.PosL, 1.0f), gWorldLightViewProj);
    vout.depthPosition = vout.position;
    return vout;
}