#pragma once
#include <glad\glad.h>
#include <glfw3.h>
#include "Observer.h"
#define KEY_DOWN GLFW_PRESS
#define KEY_UP GLFW_RELEASE
#define KEY_HOLD GLFW_REPEAT
constexpr WORD GLVersionNumber(int major, int minor) { return MAKEWORD(major, minor); }
enum events {
	ev_changeSize,
	ev_mouseMove,
	ev_keyPress
};
class Mouse {
private:
	static double x, y;
public:
	static void setPos(double x, double y);
	static double getX() { return x; }
	static double getY() { return y; }
};

class Window : public Subject
{
private:
	static Window * wind;
	GLFWwindow * window;
	int width, height;
	char * name;
	bool aa;
public:
	/**
	* Callback function to change the glViewport when window size changes
	* Do not manually call
	*/
	static void changeSizeCallback(GLFWwindow * window, int width, int height);
	/**
	* Updates the cursor positions stored in Mouse class
	* Do not manually call
	*/
	static void mouseMoveCallback(GLFWwindow * window, double x, double y);

	static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
public:
	Window(int width, int height, const char * name, WORD version = MAKEWORD(3, 3));
	~Window();
	void fullscreen(int screen_width, int screen_height);

	/**
	* Turns antialiasing on. Must be called before createWindow()
	* @param x, the number of antialiasing samples
	*/
	void antialiasing(int x);
	/**
	* Hides cursor
	* Window must already be created
	*/
	void captureCursor();
	/**
	* Shows cursor
	*/
	void releaseCursor();
	void createWindow();
	void setMouseButtonCallback(GLFWmousebuttonfun call);
	void pollEvents() { glfwPollEvents(); }
	void swapBuffers() { glfwSwapBuffers(window); }
	int getKeyState(int key);

	/**
	* @return, true if the window should close (x button etc.)
	*/
	bool windowShouldClose();

	/**
	* @param hide, if true hids the cursor, if false shows it
	*/
	void hideCursor(bool hide = true);

	void clear();
};

