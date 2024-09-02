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
						   unsigned int format, std::string_view path, bool flip, 
						   int width, int height)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(target, texID);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	if((width == 0) && (height == 0))
	{
		TextureData texInfo{loadTexture(path, flip)};
		glTexImage2D(target, 0, format, texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
		stbi_image_free(texInfo.data);
	}
	else
	{
		unsigned int srcFormat;
		if((format == GL_RGBA16F) || (format == GL_RGBA32F))
		{
			srcFormat = GL_RGBA;
		}
		else if((format == GL_RGB16F) || (format == GL_RGB32F))
		{
			srcFormat = GL_RGB;
		}
		else
		{
			srcFormat = format;
		}

		glTexImage2D(target, 0, format, width, height, 0, srcFormat, GL_UNSIGNED_BYTE, nullptr);
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(target);
	}

	return texID;
}

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view paths[6], bool flip,
						   int width, int height)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	if((width == 0) && (height == 0))
	{
		for(size_t i = 0; i < 6; i++)
		{
			TextureData texInfo{loadTexture(paths[i], flip)};
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
						 texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
			stbi_image_free(texInfo.data);
		}
	}
	else
	{
		for(unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
						 width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		}
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	return texID;
}