#include <Window.h>
#include <Resources.h>
#include "resource.h"
#include <stdio.h>
#include <Shape.h>
#include <Engine.h>
int main() {
	Window wind(1000, 800, "Engine Demo"); 
	wind.antialiasing(8);
	wind.createWindow();
	Resources res(T_1, TEST); 
	printf("%s\n", res.data);

	Camera cam(glm::vec3(0, 0, -1));
	cam.setTarget(glm::vec3(0));
	Cube box;
	Texture tex(BRICK_TEX, TEXTURE);
	box.setPos(glm::vec3(0));
	box.scale(glm::vec3(0.5));
	box.setTexture(&tex);
	Engine engine;
	wind.attatch(&engine);
	engine.perspective(glm::radians(45.f), 1000.f / 800.f, 0.1f, 50.f);
	engine.view(cam);
	Scene scene;
	scene.ADD_OBJ(box);
	while (!wind.windowShouldClose()) {
		wind.clear();
		glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
		box.rotate(glfwGetTime(), glm::vec3(0, 1, 0));
		engine.render(scene);
		wind.pollEvents();
		wind.swapBuffers();
	}
	return 0;
}