#version 330 core
out vec4 FragColor;
in vec3 texCoords;
uniform samplerCube texture_skybox;
void main(){
	FragColor = texture(texture_skybox, texCoords);
}
