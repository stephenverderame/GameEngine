#pragma once
#include "Object.h"
#include "Model.h"
#include "AnimModel.h"
//used to select which shader to use
enum class objectType {
	simple,
	model
};
struct objData {
	objectType type;
};
struct pointLight {
	glm::vec3 position;
	glm::vec3 diffuse;
	float ambientFactor;
	float specularFactor;
	float range;
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
	void addLight(pointLight * light);
	void uploadLights(Shader * s) const;
	Scene();
	~Scene();
};
#define ADD_OBJ(OBJ) addObj(&OBJ, #OBJ)
#define GET_OBJ(OBJ) [#OBJ]

