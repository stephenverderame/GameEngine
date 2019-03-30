#include "UserInput.h"
#include <unordered_map>
struct u_impl {
	std::unordered_map<keys, keyFunction> keyMaps;
};
void UserInput::update(int msg, void * data) {
	if (msg == 1) { //mouse move
		double * pos = reinterpret_cast<double*>(data);
		if (mouseInput != nullptr) {
			if (lastMousePos.firstMouse) {
				(*mouseInput)(pos[0], pos[1], pos[0], pos[1]);
				lastMousePos.firstMouse = false;
			}
			else
				(*mouseInput)(pos[0], pos[1], lastMousePos.x, lastMousePos.y);
		}
		else {
			if (lastMousePos.firstMouse) {
				defaultMouseFirstPerson(pos[0], pos[1], pos[0], pos[1]);
				lastMousePos.firstMouse = false;
			}
			else
				defaultMouseFirstPerson(pos[0], pos[1], lastMousePos.x, lastMousePos.y);
		}
		lastMousePos.x = pos[0]; lastMousePos.y = pos[1];
	}
	else if (msg == 2) { //keypress
		int * info = reinterpret_cast<int*>(data);
		if (impl->keyMaps.find((keys)info[0]) != impl->keyMaps.end()) {
			impl->keyMaps[(keys)info[0]]((actions)info[1], (keys)info[0]);
		}
		if (stdMovement && cam != nullptr && isKeyDown(info[1])) {
			switch ((keys)info[0]) {
			case keys::a:
				cam->setPos(cam->getPos() - cam->getRight() * speed);
				break;
			case keys::d:
				cam->setPos(cam->getPos() + cam->getRight() * speed);
				break;
			case keys::w:
				cam->setPos(cam->getPos() + cam->getFront() * speed);
				break;
			case keys::s:
				cam->setPos(cam->getPos() - cam->getFront() * speed);
				break;
			}
		}
	}
}
void UserInput::addKeyBind(keys k, keyFunction func) {
	impl->keyMaps.emplace(k, func);
}
void UserInput::setMouseFunction(mouseFunction f)
{
	mouseInput = &f;
}
void UserInput::setCamera(Camera * cam)
{
	this->cam = cam;
}
void UserInput::setMouseSensitivity(double s)
{
	mouseSensitivity = s;
}
double UserInput::getMouseSensitivity() const
{
	return mouseSensitivity;
}
void UserInput::standardMovement(bool enable)
{
	stdMovement = enable;
}
void UserInput::defaultMouseFirstPerson(double x, double y, double lastX, double lastY)
{
	double deltaX = x - lastX;
	double deltaY = lastY - y; //prevent inverted y
	if (cam != nullptr) {
		cam->setAngles(cam->getYaw() + deltaX * mouseSensitivity, cam->getPitch() + deltaY * mouseSensitivity);
	}
}
UserInput::UserInput() : mouseInput(nullptr), cam(nullptr), mouseSensitivity(0.1), speed(0.1) {
	impl = std::make_unique<u_impl>();
}

UserInput::~UserInput() 
{

}
