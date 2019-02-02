#include "Engine.h"

enum shader_types {
	st_common,
	st_model
};

int Engine::pickShader(objData d)
{
	switch (d.type) {
	case objectType::model:
			return st_model;
	default:
		return st_common;
	}
}

void Engine::render(const Scene & s)
{
	for (size_t i = 0; i < s.objectCount(); ++i) {
		Shader * shader = shaders[pickShader(s.objectData(i))];
		shader->use();
		s.draw(shader, i);

	}
}

void Engine::update(int msg, void * data)
{
	if (msg == 0) { //screen size changed
		//an array is passed
		int * size = reinterpret_cast<int*>(data);
		int width = size[0];
		int height = size[1];
		perspective(-1.f, (float)width / (float)height);
	}
}

void Engine::perspective(float fov, float aspectRatio, float near_plane, float far_plane)
{
	if(fov > 0)
		perspectiveData[0] = fov;
	if(aspectRatio > 0)
		perspectiveData[1] = aspectRatio;
	if(near_plane > 0)
		perspectiveData[2] = near_plane;
	if(far_plane > 0)
		perspectiveData[3] = far_plane;

	for (Shader * s : shaders) {
		s->use();
		s->setMat4("projection", glm::perspective(perspectiveData[0], perspectiveData[1], perspectiveData[2], perspectiveData[3]));
	}
}

void Engine::view(const Camera & cam)
{
	for (Shader * s : shaders) {
		s->use();
		s->setMat4("view", cam.getViewMatrix());
	}
}

Engine::Engine()
{
	shaders[st_common] = new Shader(resources, COMMON_VERT, COMMON_FRAG);
	memset(perspectiveData, 0, sizeof(perspectiveData));

}


Engine::~Engine()
{
	for (Shader * s : shaders)
		delete s;
}
