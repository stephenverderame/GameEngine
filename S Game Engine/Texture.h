#pragma once
enum class textureType {
	diffuse,
	specular,
	emission,
	normal,
	height
};
struct texOptions {
	int min, mag, wrap_s, wrap_t;
	float borderColor[4] = { -1.0f };
};
class Texture
{
private:
	unsigned int texture;
	mutable unsigned int id;
	int width, height, channels;
	textureType type;
	/**
	* Reference texture used if the instance doesn't own its own resources
	* Reference should have same life span as this instance
	* Allows a texture to reference another with the same interface and without invalid pointers
	* If the reference is set, destructor does not free texture data
	*/
	const Texture * ref;
private:
	void loadTexture(unsigned char * stbiData, texOptions * o = nullptr);
public:
	Texture(int resourceId, int resourceType, texOptions * o = nullptr);
	Texture(const char * filename, texOptions * o = nullptr);
	Texture(const Texture * r);
	void setId(int i) const { id = i; }
	int getId() const { return id; }
	void setTextureType(textureType type) { this->type = type; }
	textureType getTextureType() const { return type; }
	void bind() const;
	~Texture();
public:
	//* stbi_load
	static unsigned char * load(const char * path, int * width, int * height, int * channels);
	//* stbi_load_from_memory
	static unsigned char * load(const unsigned char * data, int length, int * width, int * height, int * channels);
};

