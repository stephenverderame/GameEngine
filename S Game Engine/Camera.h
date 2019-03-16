#pragma once
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
glm::vec3 operator*(const glm::vec3 & a, float scaler);
class Camera
{
private:
	glm::vec3 pos;
	float yaw, pitch, zoom;
	glm::vec3 target;	
	bool thirdPerson;
	mutable glm::vec3 front, right, up;
	mutable bool shouldUpdate;
	mutable glm::mat4 view;
public:
	Camera(glm::vec3 pos = glm::vec3(0), float yaw = 0, float pitch = 0) : pos(pos), yaw(yaw), pitch(pitch), thirdPerson(false), shouldUpdate(true) {};
	void setTarget(glm::vec3 target);
	void firstPerson() { shouldUpdate = true; thirdPerson = false; }

	/**
	* @return, the view matrix
	*/
	operator glm::mat4() const;
	glm::mat4 getViewMatrix() const;
	void setPos(glm::vec3 pos) { shouldUpdate = true; this->pos = pos; }
	glm::vec3 getPos() const { return pos; }
	void setAngles(float yaw, float pitch) { shouldUpdate = true; this->yaw = yaw; this->pitch = pitch; }
	float getYaw() const { return yaw; } 
	float getPitch() const { return pitch; }
	glm::vec3 getFront() const { return front; }
	glm::vec3 getRight() const { return right; }
	~Camera();
};

