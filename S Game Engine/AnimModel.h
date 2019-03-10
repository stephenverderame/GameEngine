#pragma once
#include "Object.h"
#include "Texture.h"
#include <vector>
#include <ctime>
#include <gtc\quaternion.hpp>
#include "Timer.h"
#define BONES_PER_VERTEX 4
struct bone {
	std::string name;
	glm::mat4 offsetMatrix;
	glm::mat4 finalTransformation;
};
struct animVertex {
	glm::vec3 pos, normal;
	glm::vec2 texCoord;
	unsigned int bone_ids[BONES_PER_VERTEX];
	float bone_weights[BONES_PER_VERTEX];
};
class AnimMesh {
private:
	unsigned int vao, vbo, ebo;
public:
	std::vector<animVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;
public:
	/**
	* Textures become managed by the Mesh class
	*/
	AnimMesh(std::vector<animVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
	void draw(const Shader * s) const;
	~AnimMesh();
	AnimMesh& operator=(const AnimMesh& other) = delete;
	AnimMesh(const AnimMesh& other) = delete;
};
class Animation {
private:
	glm::mat4 globalInverse;
	std::vector<bone> bones;
	std::vector<glm::mat4> boneTransforms;
	void * root, * animation;
	Timer timer;
	std::string name;
private:
	void readNodeHeirarchy(double time, void * node, glm::mat4 parentTransform);
	glm::vec3 interpolatePosition(double time, void * animationNode);
	glm::quat interpolateRotation(double time, void * animationNode);
	glm::vec3 interpolateScaling(double time, void * animationNode);
	int findBone(std::string name);
	void populateBoneTransforms(double time);
public:
	Animation(glm::mat4 gi, std::vector<bone> b, void * root, void * anim);
	void update();
	std::string getName() const { return name; }
	void bufferData(const Shader * shader) const;
	void reset() { timer.reset(); }
};
struct importer;
class AnimModel : public Object
{
private:
	std::vector<AnimMesh*> meshes;
	std::vector<std::string> loadedTextures;
	std::string texDirectory;
	glm::mat4 inverseTransform;
	std::vector<std::unique_ptr<Animation>> animations;
	//When importer destructor called, assimp scene is freed
	std::unique_ptr<importer> assimpImporter;
	size_t activeAnim;
protected:
	/**
	* Passing a void* to avoid having to include assimp headers in this header
	*/
	void loadModel(const void * scene);
	void processNode(void * node, const void * scene);
	AnimMesh * processMesh(void * aiMesh, const void * scene);
	std::vector<Texture*> loadMaterialTexture(void * material, int type, const char * name);
public:
	AnimModel(const char * path, const char * texDirectory = nullptr);
	AnimModel(int resourceId, int resourceType);
	~AnimModel();
	void draw(const Shader * s) const override;
	void activeAnimation(size_t i);
	size_t animationSize() const { return animations.size(); }
	std::unique_ptr<Animation>& fetchAnim(size_t i) { return animations[i]; }
	void activeAnimation(std::string name);
};

