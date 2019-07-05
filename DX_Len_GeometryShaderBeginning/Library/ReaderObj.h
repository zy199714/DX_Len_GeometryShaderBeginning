#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include "../common/Vertex.h"

namespace Library
{
	struct Material 
	{
		float specularexponent; // Ns ���е�����
		float opticaldensity;		// Ni ������ 
		float dissolve;				//	d	��͸���ȣ���Alphaֵ
		DirectX::XMFLOAT4 transmission;	// Tf ������
		DirectX::XMFLOAT4 ambient;		// Ka �����ⷴ����ɫ
		DirectX::XMFLOAT4 diffuse;			//	Kd ����ⷴ����ɫ
		DirectX::XMFLOAT4 specular;		//	Ks ���淴��ⷴ����ɫ
		std::wstring strTextuerDiffuse;		//	map_Kd	����ⷴ��ָ���������ļ�
	};

	struct MeshObjPart 
	{
		Material material;
		std::vector<VertexPosNormalTex> vertices;		//����
		std::vector<WORD> indices16;		//	������Ŀ
	};
 
	class ReaderMtl
	{
	public:
		bool ReadMtl(const wchar_t* mtlFileName);

	public:
		std::map<std::wstring, Material> materials;
		std::map<std::wstring, std::wstring> mapKdStrs;
	};
	class ReaderObj
	{
	public:
		bool Read(const wchar_t* objFileName);
		bool ReadObj(const wchar_t* objFileName);
		bool ReadMdo(const wchar_t* mboFileName);
		bool WriteMdo(const wchar_t* mboFileName);
	private:
		void AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni);

	public:
		std::vector<MeshObjPart> objParts;

	private:
		std::unordered_map<std::wstring, DWORD> vertexCache;
	};
}
