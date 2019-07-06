#pragma once
#include "Common.h"
#include "../common/LightHelper.h"

using namespace DirectX;

namespace Library 
{
	class Light
	{
	public:
		Light();
		~Light();

		void SetPostion(XMFLOAT3 postion);
		void SetDirection(XMFLOAT3 direction);

		virtual void SetLightAmbientColor(XMFLOAT4 color) = 0;
		virtual void SetLightDiffuseColor(XMFLOAT4 color) = 0;
		virtual void SetLightSpecularColor(XMFLOAT4 color) = 0;

		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

		XMFLOAT3 GetPostion();

		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjectionMatrix();

	private: 
		Light(const Light&);
		Light& operator=(const Light&);

	protected:
		XMFLOAT3 mPostion;		//λ��
		XMFLOAT3 mUp;				//��Դ�Ϸ�
		XMFLOAT3 mDirection;		//���䷽��
		XMFLOAT4X4 mViewMatrix;			//�ռ�任
		XMFLOAT4X4 mProjectionMatrix;	//ͶӰ����
	};

	class DirectionalLight : public Light
	{
	public:
		DirectionalLight();
		~DirectionalLight();

		void SetLightAmbientColor(XMFLOAT4 color);
		void SetLightDiffuseColor(XMFLOAT4 color);
		void SetLightSpecularColor(XMFLOAT4 color);

		void UpdateProjectionMatrix(float width, float high, float nearZ, float farZ);

		directionalLight_struct GetLight();

	private:
		DirectionalLight(const DirectionalLight&);
		DirectionalLight& operator=(const DirectionalLight&);

	private:
		directionalLight_struct mDirLight;
	};

	class PointLight :public Light
	{
	public:
		PointLight();
		~PointLight();

		void SetLightAmbientColor(XMFLOAT4 color);
		void SetLightDiffuseColor(XMFLOAT4 color);
		void SetLightSpecularColor(XMFLOAT4 color);

		void UpdateProjectionMatrix(float fovAngleY, float aspectRatio, float nearZ, float farZ);

		pointLight_struct GetLight();
	private:
		PointLight(const PointLight&);
		PointLight& operator=(const PointLight&);

	private:
		pointLight_struct mPointLight;
	};

	class SpotLight :public Light
	{
	public:
		SpotLight();
		~SpotLight();

		void SetLightAmbientColor(XMFLOAT4 color);
		void SetLightDiffuseColor(XMFLOAT4 color);
		void SetLightSpecularColor(XMFLOAT4 color);

		void UpdateProjectionMatrix(float fovAngleY, float aspectRatio, float nearZ, float farZ);

		spotLight_struct GetLight();
	private:
		SpotLight(const SpotLight&);
		SpotLight& operator=(const SpotLight&);
	private:
		spotLight_struct mSpotLight;
	};
}
