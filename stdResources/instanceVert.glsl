#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in mat4 model;

uniform mat4 projection;
uniform mat4 view;

out vec2 Tex_Coords;
out vec3 Normal;
out vec3 Frag_Pos;
out mat3 Tan_To_World;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	Tex_Coords = texCoords;
	mat3 M_Inverse = transpose(inverse(mat3(model)));
	Normal = normalize(M_Inverse * normal);
	vec3 t = normalize(M_Inverse * tangent);
	vec3 n = Normal;
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);
	Tan_To_World = mat3(t, b, n);
	Frag_Pos = vec3(model * vec4(pos, 1.0));
}
