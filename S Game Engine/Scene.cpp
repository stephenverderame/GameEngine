#include "Scene.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
struct s_impl {
	std::map<std::string, size_t> nameTable;

	//since the same object can be in multiple scenes, this class does not own each object for reusability efficiency
	std::vector<Object *> objects;
	std::vector<objData> objectDataList;
	std::vector<pointLight *> pointLights;
};


void Scene::addObj(Object * o, const char * name)
{
	pimpl->nameTable.insert(std::pair<std::string, size_t>(name, pimpl->objects.size()));
	pimpl->objects.push_back(o);
	pimpl->objectDataList.push_back({ objectType::simple });	
}

void Scene::addObj(AnimModel * m, const char * name)
{
	pimpl->nameTable.insert(std::pair<std::string, size_t>(name, pimpl->objects.size()));
	pimpl->objects.push_back(m);
	pimpl->objectDataList.push_back({ objectType::model });
}
void Scene::addLight(pointLight * light) 
{
	pimpl->pointLights.push_back(light);
}

void Scene::uploadLights(Shader * s) const 
{
	//shader must be used
	s->setInt("pointLightNum", pimpl->pointLights.size());
	for (int i = 0; i < pimpl->pointLights.size(); ++i) {
		std::stringstream ss;
		ss << "lights" << "[" << std::to_string(i) << "].";
		s->setVec3((ss.str() + "position").c_str(), pimpl->pointLights[i]->position);
		s->setVec3((ss.str() + "diffuse").c_str(), pimpl->pointLights[i]->diffuse);
//		printf("%s = %f\n", (ss.str() + "ambientFactor").c_str(), pimpl->pointLights[i]->ambientFactor);
		s->setFloat((ss.str() + "ambientFactor").c_str(), pimpl->pointLights[i]->ambientFactor);
		s->setFloat((ss.str() + "specularFactor").c_str(), pimpl->pointLights[i]->specularFactor);
		s->setFloat((ss.str() + "range").c_str(), pimpl->pointLights[i]->range);
	}
}

Object * Scene::operator[](const char * name) const
{
	return pimpl->objects[pimpl->nameTable.at(name)];
}

void Scene::draw(const Shader * s, size_t i) const
{
	//shader is used in engine render function
	pimpl->objects[i]->draw(s);
}

size_t Scene::objectCount() const
{
	return pimpl->objects.size();
}

objData Scene::objectData(size_t i) const
{
	return pimpl->objectDataList[i];
}

Scene::Scene()
{
	pimpl = std::make_unique<s_impl>();
}


Scene::~Scene()
{
}
