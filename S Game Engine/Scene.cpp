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
	std::vector<spotLight *> spotLights;
	dirLight * directionalLight;
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
void Scene::addLight(light * light) 
{
	if (light->type == lightType::spot)
		pimpl->spotLights.push_back(reinterpret_cast<spotLight *>(light));
	else if (light->type == lightType::directional)
		pimpl->directionalLight = reinterpret_cast<dirLight *>(light);
	else
		pimpl->pointLights.push_back(reinterpret_cast<pointLight*>(light));
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
	s->setInt("spotLightNum", pimpl->spotLights.size());
	for (int i = 0; i < pimpl->spotLights.size(); ++i) {
		std::stringstream ss;
		ss << "spots[" << std::to_string(i) << "].";
		s->setVec3((ss.str() + "position").c_str(), pimpl->spotLights[i]->position);
		s->setVec3((ss.str() + "direction").c_str(), pimpl->spotLights[i]->direction);
		s->setVec3((ss.str() + "color").c_str(), pimpl->spotLights[i]->color);
		s->setFloat((ss.str() + "range").c_str(), pimpl->spotLights[i]->range);
		s->setFloat((ss.str() + "cutoff").c_str(), pimpl->spotLights[i]->cutoff);
		s->setFloat((ss.str() + "outerCutoff").c_str(), pimpl->spotLights[i]->outerCutoff);
	}
	if (pimpl->directionalLight != nullptr) {
		s->setBool("useDirectional", true);
		s->setVec3("directional.direction", pimpl->directionalLight->direction);
		s->setVec3("directional.color", pimpl->directionalLight->color);
		s->setFloat("directional.ambientFactor", pimpl->directionalLight->ambientFactor);
		s->setFloat("directional.specularFactor", pimpl->directionalLight->specularFactor);
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
	pimpl->directionalLight = nullptr;
}


Scene::~Scene()
{
}
