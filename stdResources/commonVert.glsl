#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 Tex_Coords;
out vec3 Normal;
out vec3 Frag_Pos;
out mat3 Tan_To_World;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	Tex_Coords = texCoords;
	mat3 M_Inverse = mat3(transpose(inverse(model)));
	Normal = M_Inverse * normal;
	vec3 t = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 b = normalize(vec3(model * vec4(cross(normal, tangent), 0.0)));
	vec3 n = normalize(vec3(model * vec4(normal, 0.0)));
	Tan_To_World = mat3(t, b, n);
	Frag_Pos = vec3(model * vec4(pos, 1.0));
}