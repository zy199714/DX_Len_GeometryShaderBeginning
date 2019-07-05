#include "Material.h"
#include <assimp/scene.h>

namespace Library
{
	Material::Material(ID3D11Device* device, aiMaterial& material)
	{
		//InitializeTextureTypeMappings();

		//for (TextureType textureType = (TextureType)0; textureType < TextureTypeEnd; textureType = (TextureType)(textureType + 1))
		//{
		//	aiTextureType mappedTextureType = (aiTextureType)sTextureTypeMappings[textureType];

			UINT textureCount = material.GetTextureCount(aiTextureType_DIFFUSE);
			if (textureCount > 0)
			{
		//		std::vector<std::wstring>* textures = new std::vector<std::wstring>();
		//		mTextures.insert(std::pair<TextureType, std::vector<std::wstring>*>(textureType, textures));

				for (UINT textureIndex = 0; textureIndex < textureCount; textureIndex++)
				{
					aiString path;
					if (material.GetTexture(aiTextureType_DIFFUSE, textureIndex, &path) == AI_SUCCESS)
					{
						std::string w(path.C_Str());
						std::wstring wPrth;
						wPrth.assign(w.begin(), w.end());
						mTextures.push_back(wPrth);
					}
				}
			}
		//}
	}

	Material::~Material()
	{
	}

	const std::vector<std::wstring>& Material::GetTexturePaths() const
	{
		return mTextures;
	}

	UINT Material::numTexture()
	{
		return mTextures.size();
	}

	void Material::InitializeTextureTypeMappings()
	{
		if (sTextureTypeMappings.size() != TextureTypeEnd)
		{
			sTextureTypeMappings[TextureTypeDifffuse] = aiTextureType_DIFFUSE;
			sTextureTypeMappings[TextureTypeSpecularMap] = aiTextureType_SPECULAR;
			sTextureTypeMappings[TextureTypeAmbient] = aiTextureType_AMBIENT;
			sTextureTypeMappings[TextureTypeHeightmap] = aiTextureType_HEIGHT;
			sTextureTypeMappings[TextureTypeNormalMap] = aiTextureType_NORMALS;
			sTextureTypeMappings[TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
			sTextureTypeMappings[TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
			sTextureTypeMappings[TextureTypeLightMap] = aiTextureType_LIGHTMAP;
		}
	}

}