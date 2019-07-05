#pragma once

#include "Common.h"

struct aiMaterial;

namespace Library
{
	enum TextureType
	{
		TextureTypeDifffuse = 0,
		TextureTypeSpecularMap,
		TextureTypeAmbient,
		TextureTypeEmissive,
		TextureTypeHeightmap,
		TextureTypeNormalMap,
		TextureTypeSpecularPowerMap,
		TextureTypeDisplacementMap,
		TextureTypeLightMap,
		TextureTypeEnd
	};

	class Material
	{
	public:
		Material(ID3D11Device* device, aiMaterial& material);
		virtual ~Material();

		const std::vector<std::wstring>& GetTexturePaths() const;
		UINT numTexture();

		void InitializeTextureTypeMappings();

	private: 
		std::map<TextureType, UINT> sTextureTypeMappings;
		std::vector<std::wstring> mTextures;
	};
}
