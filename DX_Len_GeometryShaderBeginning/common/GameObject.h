// 简易游戏对象
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

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;

	//
	// 设置模型
	void SetModel(ID3D11Device *device, std::string fileName);

	void SetMesh(ID3D11Device *device, const Geometry::MeshData<VertexPosTex>& meshData);

	void SetPosition(float x, float y, float z);
	void SetRotate(float x, float y, float z);
	void SetScaling(float x, float y, float z);

	//
	// 绘制
	//
	
	// 绘制对象
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, MiniMapEffect& effect);
	// 绘制实例
	//void DrawInstanced(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect, const std::vector<DirectX::XMMATRIX>& data);

private:
	void CreateWorldMatrix();

	Model* mModel;												// 模型
	DirectX::XMFLOAT4X4 mWorldMatrix;							// 世界矩阵

	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRotate;
	DirectX::XMFLOAT3 mScaling;
};

#endif
