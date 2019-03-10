#version 330 core

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D texture_diffuse;

void main() {
	frag_color = texture(texture_diffuse, tex_coords);
}