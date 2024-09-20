#define STB_IMAGE_IMPLEMENTATION
#include <utility/stb_image.h>
#include <utility/texture.h>

TextureData loadTexture(std::string_view path, bool flip)
{
	int imWidth, imHeight, imNumChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* imData = stbi_load(path.data(), &imWidth, &imHeight, &imNumChannels, 4);

	if(imData == nullptr)
	{
		printf("Could not load image from %s\n", path.data());
		return {};
	}
	
	GLenum format = GL_RGBA;

	return {imWidth, imHeight, format, imData};
}

unsigned int createTexture(unsigned int target, const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view path, bool flip)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(target, texID);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	TextureData texInfo{loadTexture(path, flip)};
	glTexImage2D(target, 0, format, texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
	stbi_image_free(texInfo.data);

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(target);
	}

	return texID;
}

unsigned int createTexture(unsigned int target, const TextureParameterSet& texParams, 
						   unsigned int format, int width, int height, int numLayers)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(target, texID);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	if(numLayers == 0)
	{
		glTexStorage2D(target, log2f(width) + 1, format, width, height);
	}
	else 
	{
		glTexStorage3D(target, log2f(width) + 1, format, width, height, numLayers);
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(target);
	}

	return texID;
}				

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view paths[6], bool flip)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	for(size_t i = 0; i < 6; i++)
	{
		TextureData texInfo{loadTexture(paths[i], flip)};
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
						texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
		stbi_image_free(texInfo.data);
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	return texID;
}

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, int width, int height, int numLayers)
{
	if(numLayers == 0)
	{
		unsigned int texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texParams.texWrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texParams.texWrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texParams.texWrapR);

		for(unsigned int i = 0; i < 6; i++)
		{
			glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_TEXTURE_MAX_LEVEL, format, width, height);
		}

		if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		return texID;
	}
	else
	{
		unsigned int texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, texParams.texWrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, texParams.texWrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, texParams.texWrapR);

		glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, log2f(width) + 1, format, width, height, numLayers * 6);

		if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		return texID;
	}
}						   