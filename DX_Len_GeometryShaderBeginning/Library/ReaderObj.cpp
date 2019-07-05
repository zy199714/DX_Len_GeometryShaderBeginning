#include "ReaderObj.h"

using namespace std::experimental;

namespace Library
{
	bool ReaderObj::Read(const wchar_t* objFileName)
	{
		std::wstring file(objFileName);
		file.replace(file.end() - 4, file.end(), L".mdo");
		if (filesystem::exists(file.c_str()))
		{
			return ReadMdo(file.c_str());
		}
		else if (filesystem::exists(objFileName))
		{
			bool status =  ReadObj(objFileName);
			if (status)
			{
				return WriteMdo(file.c_str());
			}
			return status;
		}

		return false;
	}

	bool ReaderObj::ReadObj(const wchar_t* objFileName)
	{
		objParts.clear();
		vertexCache.clear();

		ReaderMtl readerMtl;

		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> texCoords;

		std::wifstream wfile(objFileName);
		std::locale chain("chs");
		wfile.imbue(chain);
		for (;;)
		{
			std::wstring wstr;
			if (!(wfile >> wstr))
			{
				break;
			}

			if (wstr[0] == '#')
			{
				//忽略注释
				while (!wfile.eof() && wfile.get() != '\n')
					continue;
			}
			else if (wstr == L"o" || wstr == L"g")
			{
				objParts.emplace_back(MeshObjPart());
				objParts.back().material.ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
				objParts.back().material.diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
				objParts.back().material.specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertexCache.clear();
			}
			else if (wstr == L"v")
			{
				DirectX::XMFLOAT3 position;
				wfile >> position.x >> position.y >> position.z;
				position.z -= position.z;
				positions.push_back(position);
			}
			else if (wstr == L"vt")
			{
				DirectX::XMFLOAT2 texCoord;
				wfile >> texCoord.x >> texCoord.y;
				texCoords.emplace_back(texCoord);
			}
			else if (wstr == L"vn")
			 {
				DirectX::XMFLOAT3 normal;
				wfile >> normal.x >> normal.y >> normal.z;
				normal.z -= normal.z;
				normals.emplace_back(normal);
			 }
			else if (wstr == L"mtllib")
			{
				std::wstring mtlfile;
				wfile >> mtlfile;
				size_t beg = 0, ed = mtlfile.size();
				// 去掉前后空格
				while (iswspace(mtlfile[beg]))
					beg++;
				while (ed > beg && iswspace(mtlfile[ed - 1]))
					ed--;
				mtlfile = mtlfile.substr(beg, ed - beg);
				// 获取路径
				std::wstring dir = objFileName;
				size_t pos;
				if ((pos = dir.find_last_of('/')) == std::wstring::npos &&
					(pos = dir.find_last_of('\\')) == std::wstring::npos)
				{
					pos = 0;
				}
				else
				{
					pos += 1;
				}

				readerMtl.ReadMtl((dir.erase(pos) + mtlfile).c_str());
			}
			else if (wstr == L"usemtl")
			{
				//
				// 使用之前指定文件内部的某一材质
				//
				std::wstring mtlName;
				std::getline(wfile, mtlName);
				// 去掉前后空格
				size_t beg = 0, ed = mtlName.size();
				while (iswspace(mtlName[beg]))
					beg++;
				while (ed > beg && iswspace(mtlName[ed - 1]))
					ed--;
				mtlName = mtlName.substr(beg, ed - beg);

				objParts.back().material = readerMtl.materials[mtlName];
			}
			else if (wstr == L"f")
			{
				VertexPosNormalTex vertex;
				DWORD vpi[3], vni[3], vti[3];
				wchar_t ignore;

				// 顶点位置索引/纹理坐标索引/法向量索引
				// 原来右手坐标系下顶点顺序是逆时针排布
				for (int i = 2; i >= 0; --i)
				{
					wfile >> vpi[i] >> ignore >> vti[i] >> ignore >> vni[i];
				}
				for (int i = 0; i < 3; ++i)
				{
					vertex.pos = positions[vpi[i] - 1];
					vertex.normal = normals[vni[i] - 1];
					vertex.tex = texCoords[vti[i] - 1];
					AddVertex(vertex, vpi[i], vti[i], vni[i]);
				}
			}
				
		}

		return true;
	}

	bool ReaderObj::ReadMdo(const wchar_t* mboFileName)
	{
		return true;
	}

	bool ReaderObj::WriteMdo(const wchar_t* mboFileName)
	{
		return true;
	}

	void ReaderObj::AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni)
	{

	}

	bool ReaderMtl::ReadMtl(const wchar_t* mtlFileName)
	{
		return true;
	}
}
