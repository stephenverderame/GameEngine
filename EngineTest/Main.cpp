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
	box.setPos(glm::vec3(1, 0, 2));
	box.scale(glm::vec3(0.2));
	box.setTexture(&tex);
	Engine engine;
	wind.attatch(&engine);
	engine.perspective(glm::radians(45.f), 1000.f / 800.f, 0.1f, 50.f);
	engine.view(cam);
	Scene scene;
	Model guy("C:\\Users\\stephen\\Downloads\\crysis-nano-suit-2\\everything\\scene.fbx");
	AnimModel guy2("C:\\Users\\stephen\\Downloads\\Assimp-Tutorial-LWJGL-3-master\\Assimp-Tutorial-LWJGL-3-master\\res\\test.dae");
	for (size_t i = 0; i < guy2.animationSize(); ++i)
		printf("%s\n", guy2.fetchAnim(i)->getName().c_str());
	Rect rect;
	rect.setColor(glm::vec4(1, 0, 0, 1));
	rect.rotate(glm::radians(-45.f), glm::vec3(0, 1, 0));
	rect.setPos(glm::vec3(-0.2, 0, 0.2));
	rect.scale(glm::vec3(0.5));
	rect.setShininess(128);
	guy2.setPos(glm::vec3(-1, -0.1, 5));
	guy2.scale(glm::vec3(0.1));
	guy.setPos(glm::vec3(0, -.5, 0));
	guy.scale(glm::vec3(0.05));
	guy.setShininess(8);
	scene.ADD_OBJ(guy);
	scene.ADD_OBJ(box);
	scene.ADD_OBJ(rect);
	pointLight p = { glm::vec3(0.3, 0, 0.3), glm::vec3(0.5, 0, 0.25), 0.2, 1.0, 1.0 };
	scene.addLight(&p);
	while (!wind.windowShouldClose()) {
		wind.clear();
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		box.rotate(glfwGetTime(), glm::vec3(0, 1, 0));
		guy.rotate(glfwGetTime(), glm::vec3(0, 1, 0));
		engine.render(scene);
		wind.pollEvents();
		wind.swapBuffers();
	}
	return 0;
}