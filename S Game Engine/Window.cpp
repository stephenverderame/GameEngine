#include "Window.h"

double Mouse::x = 0;
double Mouse::y = 0;
Window * Window::wind = nullptr;

void Window::changeSizeCallback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
	int data[2] = { width, height };
	wind->notify(ev_changeSize, data);
}

void Window::mouseMoveCallback(GLFWwindow * window, double x, double y)
{
	Mouse::setPos(x, y);
	double data[2] = { x, y };
	wind->notify(ev_mouseMove, data);
}

void Window::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	int data[4] = { key, action, scancode, mods };
	wind->notify(ev_keyPress, data);
	GLFW_KEY_ESCAPE;
}

Window::Window(int width, int height, const char * name) : width(width), height(height), name((char*)name), aa(false)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef OSX
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	wind = this;
}


Window::~Window()
{
	glfwTerminate();
}

void Window::fullscreen(int screen_width, int screen_height)
{
	glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screen_width, screen_height, GLFW_DONT_CARE);
}

void Window::antialiasing(int x)
{
	glfwWindowHint(GLFW_SAMPLES, x);
	aa = true;
}

void Window::captureCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::releaseCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::createWindow()
{
	window = glfwCreateWindow(width, height, name, NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, changeSizeCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetKeyCallback(window, keyCallback);
	if (aa)
		glEnable(GL_MULTISAMPLE);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
}

void Window::setMouseButtonCallback(GLFWmousebuttonfun call)
{
	glfwSetMouseButtonCallback(window, call);
}

int Window::getKeyState(int key)
{
	return glfwGetKey(window, key);
}

bool Window::windowShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::hideCursor(bool hide)
{
	if (hide)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Mouse::setPos(double x, double y)
{
	Mouse::x = x;
	Mouse::y = y;
}
