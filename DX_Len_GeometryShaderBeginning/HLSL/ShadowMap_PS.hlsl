#include "Basic.hlsli"

float4 PS(SHADOW_PS_INPUT vIn) : SV_TARGET
{ 
    float depth;
    float4 litcolor;

    //通过将Z像素深度除以均匀W坐标来获取像素的深度值。
    depth = vIn.depthPosition.z / vIn.depthPosition.w;

    //0.0~0.1 为红色
    if (depth<0.9f)
    {
        litcolor = float4(1.0, 0.0f, 0.0f, 1.0f);
    }
        //0.1~0.925 为蓝色
    if (depth > 0.9f)
    {
        litcolor = float4(0.0f, 1.0, 0.0f, 1.0f);
    }
        //0.925~1.0 为绿色
    if (depth > 0.925f)
    {
        litcolor = float4(0.0, 0.0f, 1.0f, 1.0f);
    }
    return litcolor;
}