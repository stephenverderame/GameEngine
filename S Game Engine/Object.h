#pragma once
#include "Shader.h"
class Object
{
protected:
	glm::vec3 pos, scalers, rotationalAxis;
	float angle;
	float shininess;
	bool instanced;
protected:
	virtual glm::mat4 calcModel() const;
public:
	Object();
	/**
	* @param s, pass the address of or pointer to a Shader. Object does not take ownership of the Shader
	*/
	virtual void draw(const Shader * s) const = 0;

	virtual void setPos(glm::vec3 pos);
	virtual void scale(glm::vec3 factors);
	virtual void rotate(float angle, glm::vec3 axis);
	virtual void setShininess(float shinyFactor);
	virtual glm::vec3 getPos() const;
	bool instancingEnabled() const { return instanced; }

};

