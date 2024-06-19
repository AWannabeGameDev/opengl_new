#define STB_IMAGE_IMPLEMENTATION
#include <utility/stb_image.h>
#include <utility/load_image.h>

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