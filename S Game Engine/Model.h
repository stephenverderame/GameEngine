#pragma once
#include "Object.h"
#include "Texture.h"
#include <vector>
#define nullbuf (~0)
struct vertex {
	glm::vec3 pos, normal, tangent;
	glm::vec2 texCoord;
};
class Mesh {
private:
	unsigned int vao, vbo, ebo, ibo;
	unsigned int instances;
public:
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;
public:
	/**
	* Textures become managed by the Mesh class
	*/
	Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, unsigned int ibo, unsigned int instances);
	void draw(const Shader * s) const;
	~Mesh();
};
class Model : public Object
{
private:
	std::vector<Mesh*> meshes;
	std::vector<std::string> loadedTextures;
	std::string texDirectory;
	unsigned int ibo = nullbuf, maxInstances = 0;
	void * instanceMap = nullptr;
protected:
	/**
	* Passing a void* to avoid having to include assimp headers in this header
	*/
	void loadModel(const void * scene);
	void processNode(void * node, const void * scene);
	Mesh * processMesh(void * aiMesh, const void * scene);
	std::vector<Texture*> loadMaterialTexture(void * material, int type);
public:
	Model(const char * path, const char * texDirectory = nullptr);
	Model(const char * path, std::vector<glm::mat4> & instanceTransforms, const char * texDirectory = nullptr);
	Model(int resourceId, int resourceType);
	~Model();
	void draw(const Shader * s) const override;
	bool updateInstance(unsigned int index, glm::mat4 & mat);
	unsigned int instanceCount() { return maxInstances; }
};

