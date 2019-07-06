//***************************************************************************************
// 简易特效管理框架
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
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;

	// 不支持复制构造
	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	// 允许转移
	IEffect(IEffect&& moveFrom) = default;
	IEffect& operator=(IEffect&& moveFrom) = default;

	virtual ~IEffect() = default;

	// 更新并绑定常量缓冲区
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

	// 获取单例
	static BasicEffect& Get();

	

	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ComPtr<ID3D11Device> device);


	// 
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);
	void SetRenderShadowMap(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);
	void XM_CALLCONV SetLightViewMatrix(DirectX::FXMMATRIX LV);
	void XM_CALLCONV SetLightProjMatrix(DirectX::FXMMATRIX LP);
	//
	// 光照、材质和纹理相关设置
	//

	// 各种类型灯光允许的最大数目
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
	// 雾效设置
	void SetFogState(bool isOn);
	void SetFogStart(float fogStart);
	void SetFogColor(DirectX::XMVECTOR fogColor);
	void SetFogRange(float fogRange);

	// 阴影设置
	void SetShadowEnabled(bool isOn);
	void SetShadowBits(float bits);


	// 应用常量缓冲区和纹理资源的变更
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

	// 获取单例
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
