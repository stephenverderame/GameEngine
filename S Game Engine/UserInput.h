#pragma once
#include "Observer.h"
#include <memory>
#include <functional>
#include "Camera.h"
enum class keys {
	a = 'A', b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, openBracket, backSlash, closeBracket, accent,
	zero = '0', one, two, three, four, five, six, seven, eight, nine,
	space = 32, quote = 39, comma = 44, dash, period, slash,
	esc = 256, enter, tab, back, insert, del, right, left, down, up, pageUp, pageDown, home, end, 
	caps = 280, scrollLock, numLock, printScreen, pause,
	f1 = 290, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
	num0 = 320, num1, num2, num3, num4, num5, num6, num7, num8, num9, numDecimal, numDivide, numMultiply, numSubtract, numAdd, numEnter, numEqual,
	lShift = 340, lCtrl, lAlt, rShift = 344, rCtrl, rAlt
};
enum class actions {
	release, press
};
struct u_impl;
class UserInput;
using keyFunction = std::function<void(actions, keys)>;
using mouseFunction = std::function<void(double, double, double, double)>;
class UserInput : public Observer
{
private:
	struct {
		double x, y;
		bool firstMouse;
	} lastMousePos {0, 0, true};
	std::unique_ptr<u_impl> impl;
	mouseFunction * mouseInput;
	Camera * cam;
	double mouseSensitivity;
	double speed;
	bool stdMovement;
public:
	UserInput();
	~UserInput();
	void update(int msg, void * data) override;
	void addKeyBind(keys k, keyFunction func);
	void setMouseFunction(mouseFunction f);
	void setCamera(Camera * cam);
	void setMouseSensitivity(double s);
	double getMouseSensitivity() const;
	void standardMovement(bool enable);
public:
	void defaultMouseFirstPerson(double x, double y, double lastX, double lastY);
};


