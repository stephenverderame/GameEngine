#pragma once
#include "Resources.h"
#include "Shader.h"
#include <array>
#include "Scene.h"
#include "Observer.h"
#include "Camera.h"
class Engine : public Observer
{
private:
	StdResources resources;
	std::array<Shader *, 1> shaders;
	float perspectiveData[4];
private:
	int pickShader(objData d);
public:
	void render(const Scene & s);
	void update(int msg, void * data) override;
	/**
	* Sets projection matrix on all shaders
	* The Projection used is a perspective matrix
	*/
	void perspective(float fov = -1.f, float aspectRatio = -1.f, float near_plane = -1.f, float far_plane = -1.f);
	/**
	* Sets view matrix on all shaders
	*/
	void view(const Camera & cam);
	Engine();
	~Engine();
};

