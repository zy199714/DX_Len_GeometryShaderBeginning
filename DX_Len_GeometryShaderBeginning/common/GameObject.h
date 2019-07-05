// ������Ϸ����
// Simple game object.
//***************************************************************************************

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "../Library/Model.h"
#include "../Library/Mesh.h"
#include "../Library/Material.h"
#include "../effect/Effects.h"
#include "../common/Geometry.h"
#include "../common/Vertex.h"

using namespace Library;

class GameObject
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();
	~GameObject();

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() const;

	//
	// ����ģ��
	void SetModel(ID3D11Device *device, std::string fileName);

	void SetMesh(ID3D11Device *device, const Geometry::MeshData<VertexPosTex>& meshData);

	void SetPosition(float x, float y, float z);
	void SetRotate(float x, float y, float z);
	void SetScaling(float x, float y, float z);

	//
	// ����
	//
	
	// ���ƶ���
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, MiniMapEffect& effect);
	// ����ʵ��
	//void DrawInstanced(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect, const std::vector<DirectX::XMMATRIX>& data);

private:
	void CreateWorldMatrix();

	Model* mModel;												// ģ��
	DirectX::XMFLOAT4X4 mWorldMatrix;							// �������

	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRotate;
	DirectX::XMFLOAT3 mScaling;
};

#endif
