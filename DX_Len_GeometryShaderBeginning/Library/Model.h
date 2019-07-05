#pragma once

#include "Common.h"
#include "Mesh.h"
#include "../common/Geometry.h"

namespace Library
{
	struct VertexIndexBuffer
	{
		UINT vertexCount;
		ID3D11ShaderResourceView* texDiffuse;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		VertexIndexBuffer()
		{
			vertexCount = 0;
			texDiffuse = nullptr;
			vertexBuffer = nullptr;
			indexBuffer = nullptr;
		}
	};

	class Material;

	class Model
	{
	public:
		Model();
		Model(ID3D11Device* device, std::string& filename, bool filpUV);

		Model(ID3D11Device *device, const Geometry::MeshData<VertexPosTex, UINT>& meshData);

		Model(const Model& model);

		~Model();

		template<class VertexType>
		void RegisterBuffer(ID3D11Device* device);
		
		bool HasMeshes() const;
		bool HasMaterials() const;
		//bool HasAnimations() const;

		const std::vector<VertexIndexBuffer*>& GetVertexIndexBuffer() const;

	private:
		std::vector<void *> mMeshes;
		std::vector<Material*> mMaterials;
		std::vector<VertexIndexBuffer*> mBuffer;
	};
}