#include "Texture.h"
#include "Resources.h"
#include <glad\glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void Texture::loadTexture(unsigned char * stbiData, texOptions * o)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (o == nullptr) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, o->wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, o->wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, o->min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, o->mag);
		if (o->borderColor[0] >= -0.3f) { //initialized to -1, anything else means border color has been set
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, o->borderColor);
		}
	}
	unsigned int format;
	switch (channels) {
	case 1:
		format = GL_RED;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RGB;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, stbiData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(stbiData);
}

Texture::Texture(int resourceId, int resourceType, texOptions * o) : id(0), type(textureType::diffuse), ref(nullptr)
{
	Resources res(resourceId, resourceType);
	loadTexture(stbi_load_from_memory((const unsigned char*)res.data, res.length, &width, &height, &channels, NULL), o);
}

Texture::Texture(const char * filename, texOptions * o) : id(0), type(textureType::diffuse), ref(nullptr)
{
	loadTexture(stbi_load(filename, &width, &height, &channels, NULL), o);
}

Texture::Texture(const Texture * r) : ref(r), type(ref->type), id(ref->id), texture(ref->texture)
{
}

void Texture::bind() const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
}

Texture::~Texture()
{
	if(ref == nullptr)
		glDeleteTextures(1, &texture);
}
