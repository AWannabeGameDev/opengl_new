#ifndef TEXTURE_H
#define TEXTURE_H

#include <string_view>
#include <utility/util.h>

struct TextureData
{
	int width, height;
	unsigned int format;
	unsigned char* data;
};

TextureData loadTexture(std::string_view path, bool flip);

struct TextureParameterSet
{
	unsigned int minFilter, magFilter;
	unsigned int texWrapS, texWrapT, texWrapR;
};

unsigned int createTexture(unsigned int target, const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view path, bool flip);

unsigned int createTexture(unsigned int target, const TextureParameterSet& texParams, 
						   unsigned int format, int width, int height, int numLayers = 0);					

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view paths[6], bool flip);

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, int width, int height, int numLayers = 0);						   

#endif