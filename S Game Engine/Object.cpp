#include "Object.h"

glm::mat4 Object::calcModel() const
{
	glm::mat4 model;
	model = glm::translate(model, pos);
	if (rotationalAxis != glm::vec3(0))
		model = glm::rotate(model, angle, rotationalAxis);
	if (scalers != glm::vec3(0))
		model = glm::scale(model, scalers);
	return model;
}

void Object::obj_init()
{
	pos = glm::vec3(0);
	scalers = glm::vec3(0);
	rotationalAxis = glm::vec3(0);
}

void Object::setPos(glm::vec3 pos)
{
	this->pos = pos;
}

void Object::scale(glm::vec3 factors)
{
	this->scalers = factors;
}

void Object::rotate(float angle, glm::vec3 axis)
{
	this->angle = angle;
	rotationalAxis = axis;
}

glm::vec3 Object::getPos() const
{
	return pos;
}

void Object::setShininess(float s) 
{
	shininess = s;
}
