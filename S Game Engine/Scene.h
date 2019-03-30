#pragma once
#include "Object.h"
#include "Model.h"
#include "AnimModel.h"
#include "Skybox.h"
//used to select which shader to use
enum class objectType {
	simple,
	model
};
struct objData {
	objectType type;
	bool instance;
};
enum class lightType {
	point,
	spot,
	directional
};
struct light {
	lightType type;
};
struct pointLight : public light {
	glm::vec3 position;
	glm::vec3 diffuse;
	float ambientFactor;
	float specularFactor;
	float range;
	pointLight() { type = lightType::point; }
	pointLight(glm::vec3 pos, glm::vec3 color, float ambient, float spec, float range) : position(pos), diffuse(color), ambientFactor(ambient),
		specularFactor(spec), range(range) { type = lightType::point; }

};
struct spotLight : public light {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float cutoff, outerCutoff, range;
	spotLight() { type = lightType::spot; }
	spotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutoff, float outerCutoff, float range)  : position(pos), direction(dir), color(color), cutoff(cutoff),
		outerCutoff(outerCutoff), range(range)
	{ type = lightType::spot; }
};
struct dirLight : public light {
	glm::vec3 position;
	glm::vec3 color;
	float ambientFactor, specularFactor;
	dirLight() { type = lightType::directional; }
	dirLight(glm::vec3 position, glm::vec3 color, float ambientFactor, float specularFactor) : position(position), color(color), ambientFactor(ambientFactor),
		specularFactor(specularFactor) { type = lightType::directional; }
};
struct s_impl;
class Scene
{
private:
	/**
	* std::map<std::string, size_t> nameTable;
	* std::vector<Object *> objects;
	* std::vector<objData> objectDataList;
	*/
	std::unique_ptr<s_impl> pimpl;
public:
	/**
	* @param o, address of or pointer to an object to draw in this scene. Not RAII for an object.
	* @param name, id to fetch this object from the scene
	*/
	void addObj(Object * o, const char * name);
	/**
	* model specefic overload in order to specify the shader to use in the objects objData
	* @see addObj(Object *, const char *)
	* @see objData
	*/
	void addObj(AnimModel * m, const char * name);
	Object * operator[](const char * name) const;
	void draw(const Shader * s, size_t i) const;
	size_t objectCount() const;
	objData objectData(size_t i) const;
	void addLight(light * light);
	void uploadLights(Shader * s) const;
	void addSkybox(Skybox * s);
	void drawSkybox(const Shader * s) const;
	Scene();
	~Scene();
};
#define ADD_OBJ(OBJ) addObj(&OBJ, #OBJ)

