#ifndef LOAD_H
#define LOAD_H

#include <string_view>
#include <utility/util.h>
#include <utility/stb_image.h>

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
						   unsigned int format, std::string_view path, bool flip, 
						   int width = 0, int height = 0);

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view paths[6], bool flip,
						   int width = 0, int height = 0);

#endif