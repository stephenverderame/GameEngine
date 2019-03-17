#include "Skybox.h"
#include <array>
#include "Resources.h"
#include "Texture.h"
enum faceIndex {
	s_right, s_left, s_up, s_down, s_back, s_front
};


void Skybox::init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenTextures(1, &skyTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
}

Skybox::Skybox(const std::string & basename)
{
	std::array<std::string, 6> faces;
	std::string fileType = basename.substr(basename.find_last_of('.'));
	std::string path = basename.substr(0, basename.find_last_of('.'));
	faces[s_right] = path + "_rt" + fileType;
	faces[s_left] = path + "_lf" + fileType;
	faces[s_up] = path + "_up" + fileType;
	faces[s_down] = path + "_dn" + fileType;
	faces[s_back] = path + "_bk" + fileType;
	faces[s_front] = path + "_ft" + fileType;

	init();

	for (int i = 0; i < faces.size(); ++i) {
		int width, height, channels;
		unsigned char * data = Texture::load(faces[i].c_str(), &width, &height, &channels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

Skybox::Skybox(const int resId, const int resType)
{
	init();
	for (int i = 0; i < 6; ++i) {
		Resources rc(resId + i, resType);
		int width, height, channels;
		unsigned char * data = Texture::load((unsigned char*)rc.data, rc.length, &width, &height, &channels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Skybox::~Skybox()
{
	glDeleteTextures(1, &skyTexture);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Skybox::draw()
{
	glDepthMask(GL_FALSE);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}
