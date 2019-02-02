#include "Shape.h"
#include <glad\glad.h>
#include <gtc\matrix_transform.hpp>
Cube::Cube()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Cube::draw(const Shader * s) const
{
	if (tex != nullptr)
		tex->bind();
	else
		s->setVec4("color", color);
	s->setMat4("model", calcModel());
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

Shape::Shape() : tex(nullptr), color(glm::vec4(0))
{
	obj_init();
}

void Shape::setTexture(Texture * tex)
{
	this->tex = tex;
}

void Shape::setColor(glm::vec4 c)
{
	color = c;
}

Shape::~Shape()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}


