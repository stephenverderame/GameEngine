#pragma once
#include "Object.h"
#include "Texture.h"
#include <vector>
struct vertex {
	glm::vec3 pos, normal;
	glm::vec2 texCoord;
};
class Mesh {
private:
	unsigned int vao, vbo, ebo;
public:
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;
public:
	/**
	* Textures become managed by the Mesh class
	*/
	Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
	void draw(const Shader * s) const;
	~Mesh();
};
class Model : public Object
{
private:
	std::vector<Mesh*> meshes;
	std::vector<std::string> loadedTextures;
	std::string texDirectory;
protected:
	/**
	* Passing a void* to avoid having to include assimp headers in this header
	*/
	void loadModel(const void * scene);
	void processNode(void * node, const void * scene);
	Mesh * processMesh(void * aiMesh, const void * scene);
	std::vector<Texture*> loadMaterialTexture(void * material, int type, const char * name);
public:
	Model(const char * path, const char * texDirectory = nullptr);
	Model(int resourceId, int resourceType);
	~Model();
	void draw(const Shader * s) const override;
};

