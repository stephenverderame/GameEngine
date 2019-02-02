#include "Shader.h"


void Shader::load(const char * vertexCode, const char * fragCode, const char * geomCode, FILE * msgOut)
{
	unsigned int v, f, g;
	int success;
	char infoLog[512];
	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vertexCode, NULL);
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(v, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &fragCode, NULL);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(f, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	if (geomCode != nullptr) {
		g = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(g, 1, &geomCode, NULL);
		glCompileShader(g);
		glGetShaderiv(g, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(g, 512, NULL, infoLog);
			fprintf(msgOut, "%s\n", infoLog);
		}
	}
	program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);
	if (geomCode != nullptr) glAttachShader(program, g);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	glDeleteShader(v);
	glDeleteShader(f);
	if (geomCode != nullptr) glDeleteShader(g);
}

Shader::Shader()
{
}

Shader::Shader(int shaderType, int vertexId, int fragId, int geomId)
{
	Resources v(vertexId, shaderType), f(fragId, shaderType);
	if (geomId != -1) {
		Resources g(geomId, shaderType);
		load(v.data, f.data, g.data);
	}
	else
		load(v.data, f.data);
}

Shader::Shader(StdResources & res, int vertexId, int fragId, int geomId)
{
	auto res1 = res.loadStdResource(vertexId, SHADER);
	auto res2 = res.loadStdResource(fragId, SHADER);
	if (geomId != -1) {
		auto res3 = res.loadStdResource(geomId, SHADER);
		load(res1->data, res2->data, res3->data);
	}
	else
		load(res1->data, res2->data);
}


Shader::~Shader()
{
	glDeleteProgram(program);
}
