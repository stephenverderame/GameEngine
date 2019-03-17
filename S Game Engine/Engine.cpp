#include "Engine.h"

enum shader_types {
	st_common,
	st_model,
	st_skybox
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
	shaders[st_common]->use();
	s.uploadLights(shaders[st_common]);
	s.drawSkybox(shaders[st_skybox]);
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
	for (int i = 0; i < shaders.size(); ++i) {
		Shader * s = shaders[i];
		s->use();
		if (i == st_skybox)
			s->setMat4("view", glm::mat4(glm::mat3(cam.getViewMatrix())));
		else
			s->setMat4("view", cam.getViewMatrix());
		s->setVec3("viewPos", cam.getPos());
	}
}

Engine::Engine()
{
	shaders[st_common] = new Shader(resources, COMMON_VERT, COMMON_FRAG);
	shaders[st_model] = new Shader(resources, MODEL_VERT, MODEL_FRAG);
	shaders[st_skybox] = new Shader(resources, SKY_VERT, SKY_FRAG);
	memset(perspectiveData, 0, sizeof(perspectiveData));
	shaders[st_common]->use();
	shaders[st_common]->setInt("texture_diffuse", DIFFUSE_TEX_ID);
	shaders[st_common]->setInt("texture_specular", SPECULAR_TEX_ID);
	shaders[st_common]->setInt("texture_normal", NORMAL_TEX_ID);
	shaders[st_skybox]->use();
	shaders[st_skybox]->setInt("texture_skybox", 0);

}


Engine::~Engine()
{
	for (Shader * s : shaders)
		delete s;
}

