#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 Tex_Coords;
out vec3 Normal;
out vec3 Frag_Pos;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	Tex_Coords = texCoords;
	Normal = mat3(transpose(inverse(model))) * normal;
	Frag_Pos = vec3(model * vec4(pos, 1.0));
}