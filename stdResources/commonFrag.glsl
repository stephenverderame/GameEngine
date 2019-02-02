#version 330 core

in vec2 tex_coords;
out vec4 fragColor;

uniform vec4 color = {-1.0, -1.0, -1.0, 1.0};
uniform sampler2D tex;

void main(){
	if(color.r < -0.2)
		fragColor = texture(tex, tex_coords);
	else
		fragColor = color;
}