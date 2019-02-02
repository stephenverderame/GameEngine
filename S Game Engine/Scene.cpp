#include "Scene.h"
#include <vector>
#include <map>
#include <string>
struct s_impl {
	std::map<std::string, size_t> nameTable;

	//since the same object can be in multiple scenes, this class does not own each object for reusability efficiency
	std::vector<Object *> objects;
	std::vector<objData> objectDataList;
};


void Scene::addObj(Object * o, const char * name)
{
	pimpl->nameTable.insert(std::pair<std::string, size_t>(name, pimpl->objects.size()));
	pimpl->objects.push_back(o);
	pimpl->objectDataList.push_back({ objectType::simple });	
}

void Scene::addObj(Model * m, const char * name)
{
	pimpl->nameTable.insert(std::pair<std::string, size_t>(name, pimpl->objects.size()));
	pimpl->objects.push_back(m);
	pimpl->objectDataList.push_back({ objectType::model });
}

Object * Scene::operator[](const char * name) const
{
	return pimpl->objects[pimpl->nameTable.at(name)];
}

void Scene::draw(const Shader * s, size_t i) const
{
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
