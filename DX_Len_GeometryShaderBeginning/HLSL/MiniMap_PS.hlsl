#include "Minimap.hlsli"

// ������ɫ��
float4 PS(VertexPosHTex pIn) : SV_Target
{
    float4 color = gTex.Sample(gSam, pIn.Tex);
    
    return color;
}