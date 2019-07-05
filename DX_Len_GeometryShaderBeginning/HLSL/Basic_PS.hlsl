#include "Basic.hlsli"

// ������ɫ��(3D)
float4 PS(VertexPosHWNormalTex pIn) : SV_Target
{ 
    // ����ʹ��������ʹ��Ĭ�ϰ�ɫ
    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (gTextureUsed)
    {
        texColor = gDiffuseMap.Sample(gSamWrap, pIn.Tex);
        // ��ǰ���вü����Բ�����Ҫ������ؿ��Ա����������
        clip(texColor.a - 0.1f);
    }

    // ��Ӱ����
    [flatten]
    if(gShadowEnabled)
    {
        float4 lpos = float4(pIn.ProjTex.xyz / pIn.ProjTex.w, pIn.ProjTex.w);

        if (lpos.x < -1.0f || lpos.x > 1.0f ||
            lpos.y < -1.0f || lpos.y > 1.0f ||
            lpos.z < 0.0f || lpos.z > 1.0f)
        {
        }
        else
        {     
            lpos.x = lpos.x / 2 + 0.5;
            lpos.y = lpos.y / 2 + 0.5;

            float shadowMapDepth = gShadowMap.Sample(gSamWrap, float2(lpos.x, lpos.y));
            //float shadowMapDepth = length(gLightPos.xyz - pIn.PosW);

            if (shadowMapDepth < lpos.z)
                return texColor * 0.55;
        }
    }
    
    // ��׼��������
    pIn.NormalW = normalize(pIn.NormalW);

    // �������ָ���۾����������Լ��������۾��ľ���
    float3 toEyeW = normalize(gEyePosW - pIn.PosW);
    float distToEye = distance(gEyePosW, pIn.PosW);

    // ��ʼ��Ϊ0 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    int i;

    [unroll]
    for (i = 0; i < gNumDirLight; ++i)
    {
        ComputeDirectionalLight(gMaterial, gDirLight[i], pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
        
    [unroll]
    for (i = 0; i < gNumPointLight; ++i)
    {
        ComputePointLight(gMaterial, gPointLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    [unroll]
    for (i = 0; i < gNumSpotLight; ++i)
    {
        ComputeSpotLight(gMaterial, gSpotLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
  
    float4 litColor = texColor * (ambient + diffuse) + spec;
    
    // ��Ч����
    [flatten]
    if (gFogEnabled)
    {
        // �޶���0.0f��1.0f��Χ
        float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
        // ������ɫ�͹�����ɫ�������Բ�ֵ
        litColor = lerp(litColor, gFogColor, fogLerp);
    }

    litColor.a = texColor.a * gMaterial.Diffuse.a;
    return litColor;
}
