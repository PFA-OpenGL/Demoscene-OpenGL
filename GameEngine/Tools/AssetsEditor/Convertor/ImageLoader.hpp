#pragma once

#include "AssimpLoader.hpp"
#include <Texture/Texture.hpp>
#include <SOIL.h>

namespace AGE
{
	class ImageLoader
	{
	public:
		static bool save(AssetDataSet &dataSet)
		{
			if (dataSet.texturesLoaded == false)
				return false;
			for (auto &m : dataSet.textures)
			{
				auto folderPath = std::tr2::sys::path(dataSet.serializedDirectory.path().directory_string() + "\\" + File(m->rawPath).getFolder());

				if (!std::tr2::sys::exists(folderPath) && !std::tr2::sys::create_directories(folderPath))
				{
					std::cerr << "Material convertor error : creating directory" << std::endl;
					return false;
				}
				auto name = dataSet.serializedDirectory.path().directory_string() + "\\" + File(m->rawPath).getFolder() + "\\" + File(m->rawPath).getShortFileName() + ".tage";
				std::ofstream ofs(name, std::ios::trunc | std::ios::binary);
				cereal::PortableBinaryOutputArchive ar(ofs);
				ar(*m);
			}
			return true;
		}

		static bool load(AssetDataSet &dataSet)
		{
			dataSet.texturesLoaded = false;
			if (!dataSet.assimpScene)
			{
				if (!AssimpLoader::Load(dataSet))
					return false;
			}

			if (dataSet.assimpScene->HasTextures())
			{
				for (auto textureIndex = 0; textureIndex < dataSet.assimpScene->mNumTextures; ++textureIndex)
				{
					auto &aiText = dataSet.assimpScene->mTextures[textureIndex];
					unsigned int i = 0;
				}
			}

			for (auto &e : dataSet.texturesPath)
			{
				bool found = false;
				for (auto &f : dataSet.textures)
				{
					if (f->rawPath == e)
					{
						found = true;
						break;
					}
				}
				if (found)
					continue;
				auto path = dataSet.rawDirectory.path().string() + "\\" + e;
				int width, height, channels;
				auto imgData = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
				if (imgData)
				{
					auto t = new TextureData();
					dataSet.textures.push_back(t);
					t->width = width;
					t->height = height;
					t->rawPath = e;
					t->data.assign(imgData, imgData + (sizeof(unsigned char) * width * height * channels));
					SOIL_free_image_data(imgData);
				}
			}
			dataSet.texturesPath.clear();
			if (dataSet.textures.size() == 0)
			{
				std::cerr << "ImageLoader : Image has not been loaded" << std::endl;
				return false;
			}
			dataSet.texturesLoaded = true;
			return true;
		}
	};
}