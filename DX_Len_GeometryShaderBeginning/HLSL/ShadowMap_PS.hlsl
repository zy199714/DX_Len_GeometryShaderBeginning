#include "Basic.hlsli"

float4 PS(SHADOW_PS_INPUT vIn) : SV_TARGET
{ 
    float depth;
    float4 litcolor;

    //ͨ����Z������ȳ��Ծ���W��������ȡ���ص����ֵ��
    depth = vIn.depthPosition.z / vIn.depthPosition.w;

    //0.0~0.1 Ϊ��ɫ
    if (depth<0.9f)
    {
        litcolor = float4(1.0, 0.0f, 0.0f, 1.0f);
    }
        //0.1~0.925 Ϊ��ɫ
    if (depth > 0.9f)
    {
        litcolor = float4(0.0f, 1.0, 0.0f, 1.0f);
    }
        //0.925~1.0 Ϊ��ɫ
    if (depth > 0.925f)
    {
        litcolor = float4(0.0, 0.0f, 1.0f, 1.0f);
    }
    return litcolor;
}