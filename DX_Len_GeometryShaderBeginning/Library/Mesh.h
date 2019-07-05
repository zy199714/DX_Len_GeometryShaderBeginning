#pragma once

#include "Common.h"
#include "../common/Vertex.h"
#include "../common/Geometry.h"
#include <assimp/scene.h>

using namespace DirectX;

struct aiMesh;

namespace Library
{
	class Material;

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	class Mesh
	{
		friend class Model;

	public:
		Mesh(const aiMesh* mesh)
		{
			//Vertices
			mVertices.reserve(mesh->mNumVertices);
			for (UINT i = 0; i < mesh->mNumVertices; i++)
			{
				aiVector3D vertex = mesh->mVertices[i];
				mVertices.push_back(XMFLOAT3(vertex.x, vertex.y, vertex.z));
			}

			// Normals
			if (mesh->HasNormals())
			{
				mNormals.reserve(mesh->mNumVertices);
				for (UINT i = 0; i < mesh->mNumVertices; i++)
				{
					aiVector3D normal = mesh->mNormals[i];
					mNormals.push_back(XMFLOAT3(normal.x, normal.y, normal.z));
				}
			}

			// Tangents
			if (mesh->HasTangentsAndBitangents())
			{
				mTangents.reserve(mesh->mNumVertices);
				for (UINT i = 0; i < mesh->mNumVertices; i++)
				{
					aiVector3D tangents = mesh->mTangents[i];
					aiVector3D bitangents = mesh->mBitangents[i];
					mTangents.push_back(XMFLOAT3(tangents.x, tangents.y, tangents.z));
				}
			}

			// Texture Coordinates
			UINT uvChannelCount = mesh->GetNumUVChannels();
			for (UINT i = 0; i < uvChannelCount; i++)
			{
				std::vector<XMFLOAT3>* textureCoordinates = new std::vector<XMFLOAT3>();
				textureCoordinates->reserve(mesh->mNumVertices);
				mTextureCoordinates.push_back(textureCoordinates);

				aiVector3D* aiTextureCoordinates = mesh->mTextureCoords[i];
				for (UINT j = 0; j < mesh->mNumVertices; j++)
				{
					aiVector3D texturecoord = aiTextureCoordinates[j];
					textureCoordinates->push_back(XMFLOAT3(texturecoord.x, texturecoord.y, texturecoord.z));
				}
			}

			// Vertex Colors
			UINT colorChannelCount = mesh->GetNumColorChannels();
			for (UINT i = 0; i < colorChannelCount; i++)
			{
				std::vector<XMFLOAT4>* vertexColors = new std::vector<XMFLOAT4>();
				vertexColors->reserve(mesh->mNumVertices);
				mVertexColors.push_back(vertexColors);

				aiColor4D* aiVertexColors = mesh->mColors[i];
				for (UINT j = 0; j < mesh->mNumVertices; j++)
				{
					aiColor4D vertexcolor = aiVertexColors[j];
					vertexColors->push_back(XMFLOAT4(vertexcolor.r, vertexcolor.g, vertexcolor.b, vertexcolor.a));
				}
			}

			// Faces (note: could pre-reserve if we limit primitive types)
			if (mesh->HasFaces())
			{
				mFaceCount = mesh->mNumFaces;
				for (UINT i = 0; i < mFaceCount; i++)
				{
					aiFace* face = &mesh->mFaces[i];

					for (UINT j = 0; j < face->mNumIndices; j++)
					{
						mIndices.push_back(face->mIndices[j]);
					}
				}
			}

			mMaterialIndex = mesh->mMaterialIndex;

			VertexPosNormalTex verPosNorTex;
			for (UINT i = 0; i < mVertices.size(); i++)
			{
				verPosNorTex.pos = mVertices.at(i);
				verPosNorTex.normal = mNormals.at(i);
				XMFLOAT3 texCoord = mTextureCoordinates.at(0)->at(i);
				verPosNorTex.tex = XMFLOAT2(texCoord.x, texCoord.y);

				mVertexPosNormalTex.push_back(verPosNorTex);
			}
		}
		 
		Mesh(const Geometry::MeshData<VertexType, IndexType>& meshData)
		{
			mVertexPosNormalTex = meshData.vertexVec;
			mIndices = meshData.indexVec;
		}

		~Mesh()
		{
			for (std::vector<DirectX::XMFLOAT3>* textureCoordinates : mTextureCoordinates)
			{
				delete textureCoordinates;
			}

			for (std::vector<DirectX::XMFLOAT4>* vertexColors : mVertexColors)
			{
				delete vertexColors;
			}
		}

		const UINT GetVertexCount()
		{
			return mIndices.size();
		}
 
		std::vector<VertexType>* GetVertex()
		{
			return &mVertexPosNormalTex;
		}

		std::vector<UINT>* GetIndices()
		{
			return &mIndices;
		}

	private:
		Mesh(const Mesh& rhs);

		std::vector<DirectX::XMFLOAT3> mVertices;
		std::vector<DirectX::XMFLOAT3> mNormals;
		std::vector<DirectX::XMFLOAT3> mTangents;
		std::vector<DirectX::XMFLOAT3> mBitangents;
		std::vector<std::vector<DirectX::XMFLOAT3>*> mTextureCoordinates;
		std::vector<std::vector<DirectX::XMFLOAT4>*> mVertexColors;
		UINT mFaceCount;
		std::vector<UINT> mIndices;
		UINT mMaterialIndex;

		std::vector<VertexType> mVertexPosNormalTex;
	};
}