#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <cstring>
#include <DirectXMath.h>


// ������
struct directionalLight_struct
{
	directionalLight_struct() { memset(this, 0, sizeof(directionalLight_struct)); }

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Direction;
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// ���
struct pointLight_struct
{
	pointLight_struct() { memset(this, 0, sizeof(pointLight_struct)); }

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	// �����4D����: (Position, Range)
	DirectX::XMFLOAT3 Position;
	float Range;

	// �����4D����: (A0, A1, A2, Pad)
	DirectX::XMFLOAT3 Att;
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// �۹��
struct spotLight_struct
{
	spotLight_struct() { memset(this, 0, sizeof(spotLight_struct)); }

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	// �����4D����: (Position, Range)
	DirectX::XMFLOAT3 Position;
	float Range;

	// �����4D����: (Direction, Spot)
	DirectX::XMFLOAT3 Direction;
	float Spot;

	// �����4D����: (Att, Pad)
	DirectX::XMFLOAT3 Att;
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// ����������
struct material_struct
{
	material_struct() { memset(this, 0, sizeof(material_struct)); }

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular; // w = ���淴��ǿ��
	DirectX::XMFLOAT4 Reflect;
};

#endif