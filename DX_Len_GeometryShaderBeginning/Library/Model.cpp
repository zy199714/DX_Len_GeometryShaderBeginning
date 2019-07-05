#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "../common/DDSTextureLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Library
{
	Model::Model(ID3D11Device* device, std::string& filename, bool filpUV)
		:mMeshes(),mMaterials()
	{
		Assimp::Importer importer;

		UINT flags = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FlipWindingOrder
					| aiProcess_MakeLeftHanded | aiProcess_ConvertToLeftHanded;
		if (filpUV)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(filename,flags);

		if (scene == nullptr)
		{
			throw importer.GetErrorString();
		}

		if (scene->HasMaterials())
		{
			for (UINT i = 0; i < scene->mNumMaterials; i++)
			{
				Material * material = new Material(device, *(scene->mMaterials[i]));
				if(material != nullptr)
					mMaterials.push_back(material);
			}
		}

		if (scene->HasMeshes())
		{
			for (UINT i = 0; i < scene->mNumMeshes; i++)
			{
				Mesh<>* mesh = new Mesh<>(scene->mMeshes[i]);
				if (mesh !=nullptr)
					mMeshes.push_back(mesh);
			}
		}

		RegisterBuffer<VertexPosNormalTex>(device);
	}

	Model::Model(ID3D11Device *device, const Geometry::MeshData<VertexPosTex, UINT>& meshData)
		:mMeshes(), mMaterials()
	{
		Mesh<VertexPosTex, UINT>* mesh = new Mesh<VertexPosTex, UINT>(meshData);

		if (mesh != nullptr)
			mMeshes.push_back(mesh);

		RegisterBuffer<VertexPosTex>(device);
	}

	Model::Model()
	{

	}

	Model::Model(const Model& model)
		:mMeshes(model.mMeshes), mMaterials(model.mMaterials)
	{
		
	}

	Model::~Model()
	{
		for (auto mesh : mMeshes)
		{
			delete mesh;
		}

		for (Material* material : mMaterials)
		{
			delete material;
		}

		for (VertexIndexBuffer* buffer : mBuffer)
		{
			delete buffer;
		}
	}

	template<class VertexType>
	void Model::RegisterBuffer(ID3D11Device* device)
	{
		UINT count = mMeshes.size();
		for (UINT i = 0; i < count; i++)
		{
			auto mesh = (Mesh<VertexType>*)mMeshes.at(i);
			VertexIndexBuffer* buffer = new VertexIndexBuffer;

			buffer->vertexCount = mesh->GetVertexCount();

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = sizeof(VertexType) * buffer->vertexCount;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA subresource;
			ZeroMemory(&subresource, sizeof(D3D10_SUBRESOURCE_DATA));
			subresource.pSysMem = mesh->GetVertex()->data();

			device->CreateBuffer(&bufferDesc, &subresource, &buffer->vertexBuffer);

			bufferDesc.ByteWidth = sizeof(UINT) * buffer->vertexCount;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			subresource.pSysMem = mesh->GetIndices()->data();

			device->CreateBuffer(&bufferDesc, &subresource, &buffer->indexBuffer);

			if (mMaterials.size() > 0)
			{
				std::vector<std::wstring> mTextures = mMaterials.at(mesh->mMaterialIndex)->GetTexturePaths();
				if (mTextures.size() > 0)
				{
					std::wstring path = L"Model/" + mTextures.at(0);
					DirectX::CreateDDSTextureFromFile(device, path.c_str(), nullptr, &buffer->texDiffuse);
				}
			}

			
			mBuffer.push_back(buffer);
		}
	}

	bool Model::HasMeshes() const
	{
		return (mMeshes.size() > 0);
	}

	bool Model::HasMaterials() const
	{
		return (mMaterials.size() > 0);
	}

	const std::vector<VertexIndexBuffer*>& Model::GetVertexIndexBuffer() const
	{
		return mBuffer;
	}
}