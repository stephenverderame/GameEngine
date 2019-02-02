        ��  ��                  L      �����
     0 	        #version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 tex_coords;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	tex_coords = texCoords;
}�       �����     0 	        #version 330 core

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