//***************************************************************************************
// ������Ч������
// Simple effect management framework.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include <memory>
#include "../common/LightHelper.h"
#include "../common/RenderStates.h"


class IEffect
{
public:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;

	// ��֧�ָ��ƹ���
	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	// ����ת��
	IEffect(IEffect&& moveFrom) = default;
	IEffect& operator=(IEffect&& moveFrom) = default;

	virtual ~IEffect() = default;

	// ���²��󶨳���������
	virtual void Apply(ComPtr<ID3D11DeviceContext> deviceContext) = 0;
};


class BasicEffect : public IEffect
{
public:

	enum RenderType { RenderObject, RenderInstance };

	BasicEffect();
	virtual ~BasicEffect() override;

	BasicEffect(BasicEffect&& moveFrom);
	BasicEffect& operator=(BasicEffect&& moveFrom);

	// ��ȡ����
	static BasicEffect& Get();

	

	// ��ʼ��Basic.hlsli������Դ����ʼ����Ⱦ״̬
	bool InitAll(ComPtr<ID3D11Device> device);


	// 
	// ��Ⱦģʽ�ı��
	//

	// Ĭ��״̬������
	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);
	void SetRenderShadowMap(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);

	//
	// ��������
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);
	void XM_CALLCONV SetLightViewMatrix(DirectX::FXMMATRIX LV);
	void XM_CALLCONV SetLightProjMatrix(DirectX::FXMMATRIX LP);
	//
	// ���ա����ʺ������������
	//

	// �������͵ƹ�����������Ŀ
	static const int maxLights = 5;

	void SetDirLight(size_t pos, const directionalLight_struct& dirLight);
	void SetPointLight(size_t pos, const pointLight_struct& pointLight);
	void SetSpotLight(size_t pos, const spotLight_struct& spotLight);
	void SetNumLight(int dir, int point, int spot);

	void SetMaterial(const material_struct& mat);

	void SetTextureUsed(bool isUsed);

	void SetTextureDiffuse(ComPtr<ID3D11ShaderResourceView> texture);
	void SetTextureShadow(ComPtr<ID3D11ShaderResourceView> texture);

	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos);
	void XM_CALLCONV SetLightPos(DirectX::FXMVECTOR lightPos);
	
	//
	// ��Ч����
	void SetFogState(bool isOn);
	void SetFogStart(float fogStart);
	void SetFogColor(DirectX::XMVECTOR fogColor);
	void SetFogRange(float fogRange);

	// ��Ӱ����
	void SetShadowEnabled(bool isOn);
	void SetShadowBits(float bits);


	// Ӧ�ó�����������������Դ�ı��
	void Apply(ComPtr<ID3D11DeviceContext> deviceContext);
	
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

class MiniMapEffect : public IEffect
{
public:	
	MiniMapEffect();
	virtual ~MiniMapEffect() override;

	MiniMapEffect(MiniMapEffect&& moveFrom);
	MiniMapEffect& operator=(MiniMapEffect&& moveFrom);

	// ��ȡ����
	static MiniMapEffect& Get();

	bool InitAll(ComPtr<ID3D11Device> device);

	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext);

	void SetTextureDiffuse(ComPtr<ID3D11ShaderResourceView> texture);

	void Apply(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};
#endif
