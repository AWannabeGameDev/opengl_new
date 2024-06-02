#ifndef LOAD_H
#define LOAD_H

#include <string_view>
#include <utility/util.h>

struct TextureData
{
	int width, height;
	unsigned int format;
	unsigned char* data;
};

TextureData loadTexture(std::string_view path, bool flip);

#endif