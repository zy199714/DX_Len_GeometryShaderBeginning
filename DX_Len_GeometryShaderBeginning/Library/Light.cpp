#include "Light.h"

namespace Library
{
	Light::Light()
		:mPostion(0.0f,0.0f,0.0f),mUp(0.0f, 1.0f, 0.0f),mDirection(0.0f, 0.0f, 1.0f)
	{

	}

	Light::~Light()
	{

	}

	void Light::SetPostion(XMFLOAT3 postion)
	{
		mPostion = postion;
	}

	void Light::SetDirection(XMFLOAT3 direction)
	{
		XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&direction));
		XMVECTOR up = XMLoadFloat3(&mUp);

		XMVECTOR rig = XMVector3Normalize(XMVector2Cross(dir, up));
		up = XMVector3Normalize(XMVector2Cross(dir, rig));

		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mDirection, dir);
	}

	void Light::UpdateViewMatrix()
	{
		XMVECTOR postion = XMLoadFloat3(&mPostion);
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);

		XMMATRIX viewMatrix = XMMatrixLookToLH(postion, direction, up);
		XMStoreFloat4x4(&mViewMatrix, viewMatrix);
	}

	void Light::UpdateProjectionMatrix()
	{

	}

	XMFLOAT3 Light::GetPostion()
	{
		return mPostion;
	}

	XMMATRIX Light::GetViewMatrix()
	{
		return XMLoadFloat4x4(&mViewMatrix);
	}

	XMMATRIX Light::GetProjectionMatrix()
	{
		return XMLoadFloat4x4(&mProjectionMatrix);
	}

	/////////////////
	//DirectionalLight
	///////////////////
	DirectionalLight::DirectionalLight()
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	void DirectionalLight::SetLightAmbientColor(XMFLOAT4 color)
	{
		mDirLight.Ambient = color;
	}

	void DirectionalLight::SetLightDiffuseColor(XMFLOAT4 color)
	{
		mDirLight.Diffuse = color;
	}

	void DirectionalLight::SetLightSpecularColor(XMFLOAT4 color)
	{
		mDirLight.Specular = color;
	}

	void DirectionalLight::UpdateProjectionMatrix(float width, float high, float nearZ, float farZ)
	{
		XMMATRIX projectionMatrix = XMMatrixOrthographicLH(width, high, nearZ, farZ);
		XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
	}

	directionalLight_struct DirectionalLight::GetLight()
	{
		return mDirLight;
	}

	///////////////////////////////
	//PointLight
	/////////////////////////////////
	PointLight::PointLight()
	{

	}

	PointLight::~PointLight()
	{

	}

	void PointLight::SetLightAmbientColor(XMFLOAT4 color)
	{
		mPointLight.Ambient = color;
	}

	void PointLight::SetLightDiffuseColor(XMFLOAT4 color)
	{
		mPointLight.Diffuse = color;
	}

	void PointLight::SetLightSpecularColor(XMFLOAT4 color)
	{
		mPointLight.Specular = color;
	}

	void PointLight::UpdateProjectionMatrix(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
	}

	pointLight_struct PointLight::GetLight()
	{
		return mPointLight;
	}

	///////////////////////////////////
	// spotLight
	//////////////////////////////////

	SpotLight::SpotLight()
	{

	}

	SpotLight::~SpotLight()
	{

	}

	void SpotLight::SetLightAmbientColor(XMFLOAT4 color)
	{
		mSpotLight.Ambient = color;
	}

	void SpotLight::SetLightDiffuseColor(XMFLOAT4 color)
	{
		mSpotLight.Diffuse = color;
	}

	void SpotLight::SetLightSpecularColor(XMFLOAT4 color)
	{
		mSpotLight.Specular = color;
	}

	void SpotLight::UpdateProjectionMatrix(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
	}

	spotLight_struct SpotLight::GetLight()
	{
		return mSpotLight;
	}

}