#include "Camera.h"


void Camera::setTarget(glm::vec3 target)
{
	thirdPerson = true;
	shouldUpdate = true;
	this->target = target;
}

Camera::operator glm::mat4() const
{
	if (shouldUpdate) {
		if (!thirdPerson) {
			glm::vec3 f;
			f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			f.y = sin(glm::radians(pitch));
			f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			front = glm::normalize(f);
			right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
			up = glm::normalize(glm::cross(right, front));

			view = glm::lookAt(pos, pos + front, up);
		}
		else {
			front = glm::normalize(target - pos);
			right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
			up = glm::normalize(glm::cross(right, front));
			view = glm::lookAt(pos, target, up);
		}
		shouldUpdate = false;
	}
	return view;



}

glm::mat4 Camera::getViewMatrix() const
{
	return (glm::mat4)(*this);
}

Camera::~Camera()
{
}
