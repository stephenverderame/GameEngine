#version 330 core
#define MAX_BONES 200
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 boneWeights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 bones[MAX_BONES];

out vec2 tex_coords;

void main(){

	mat4 boneTransform = bones[uint(boneIds.x)] * boneWeights.x +
						 bones[uint(boneIds.y)] * boneWeights.y +
						 bones[uint(boneIds.z)] * boneWeights.z +
						 bones[uint(boneIds.w)] * boneWeights.w;

	vec4 p_pos = boneTransform * vec4(pos, 1.0);
	gl_Position = projection * view * model * p_pos;
	tex_coords = texCoords;

}