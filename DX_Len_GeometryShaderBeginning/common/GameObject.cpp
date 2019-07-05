#include "GameObject.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace Library;
using namespace DirectX;

struct InstancedData
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
};

GameObject::GameObject()
	: mModel(), mPosition(0, 0, 0), mRotate(0, 0, 0), mScaling(1,1,1), mWorldMatrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)
{

}


GameObject::~GameObject()
{
	DeleteObject(mModel);
}

XMFLOAT3 GameObject::GetPosition() const
{
	return mPosition;
}

void GameObject::SetModel(ID3D11Device *device, std::string fileName)
{
	mModel = new Model(device, fileName, false);
}


void GameObject::SetMesh(ID3D11Device * device, const Geometry::MeshData<VertexPosTex>& meshData)
{
	mModel = new Model(device, meshData);
}

void GameObject::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}


void GameObject::SetRotate(float x, float y, float z)
{
	mRotate = XMFLOAT3(x, y, z);
}


void GameObject::SetScaling(float x, float y, float z)
{
	mScaling = XMFLOAT3(x, y, z);
}

void GameObject::Draw(ComPtr<ID3D11DeviceContext> deviceContext, MiniMapEffect & effect)
{
	UINT strides = sizeof(VertexPosTex);
	UINT offest = 0;
 
	std::vector<VertexIndexBuffer*> meshvector = mModel->GetVertexIndexBuffer();

	for (VertexIndexBuffer* buffer : meshvector)
	{ 
		deviceContext->IASetVertexBuffers(0, 1, &buffer->vertexBuffer, &strides, &offest);
		deviceContext->IASetIndexBuffer(buffer->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(buffer->vertexCount, 0, 0);
	}
}

void GameObject::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect)
{
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offest = 0;

	UINT i = 0;
	std::vector<VertexIndexBuffer*> meshvector = mModel->GetVertexIndexBuffer();

	for (VertexIndexBuffer* buffer : meshvector)
	{
		CreateWorldMatrix();
		effect.SetWorldMatrix(XMLoadFloat4x4(&mWorldMatrix));
		ID3D11ShaderResourceView* srv = buffer->texDiffuse;
		if (srv != nullptr)
		{
			effect.SetTextureUsed(true);
			effect.SetTextureDiffuse(srv);
		}
		else
		{
			effect.SetTextureUsed(false);
		}

		deviceContext->IASetVertexBuffers(0, 1, &buffer->vertexBuffer, &strides, &offest);
		deviceContext->IASetIndexBuffer(buffer->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(buffer->vertexCount, 0, 0);
	}
}

void GameObject::CreateWorldMatrix()
{
	XMVECTOR position = XMLoadFloat3(&mPosition);
	XMVECTOR rotate = XMLoadFloat3(&mRotate);
	XMVECTOR scaling = XMLoadFloat3(&mScaling);
	XMMATRIX world = XMMatrixTranslationFromVector(position) * 
		XMMatrixRotationRollPitchYawFromVector(rotate) * XMMatrixScalingFromVector(scaling);
	XMStoreFloat4x4(&mWorldMatrix, world);
}
