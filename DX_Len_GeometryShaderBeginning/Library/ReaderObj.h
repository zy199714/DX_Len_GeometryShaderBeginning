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
		float specularexponent; // Ns 集中的亮点
		float opticaldensity;		// Ni 折射率 
		float dissolve;				//	d	不透明度，即Alpha值
		DirectX::XMFLOAT4 transmission;	// Tf 过滤器
		DirectX::XMFLOAT4 ambient;		// Ka 环境光反射颜色
		DirectX::XMFLOAT4 diffuse;			//	Kd 漫射光反射颜色
		DirectX::XMFLOAT4 specular;		//	Ks 镜面反射光反射颜色
		std::wstring strTextuerDiffuse;		//	map_Kd	漫射光反射指定的纹理文件
	};

	struct MeshObjPart 
	{
		Material material;
		std::vector<VertexPosNormalTex> vertices;		//顶点
		std::vector<WORD> indices16;		//	顶点数目
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
