        ��  ��                        �����
     0 	        #version 330 core
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
}  �      �����     0 	        #version 330 core
#define MAX_POINT_LIGHTS 64

in vec2 Tex_Coords;
in vec3 Normal;
in vec3 Frag_Pos;
in mat3 Tan_To_World;

out vec4 fragColor;

uniform vec4 color = {-1.0, -1.0, -1.0, 1.0};
uniform sampler2D texture_diffuse;
uniform bool useSpecMap;
uniform sampler2D texture_specular;
uniform bool useNormalMap = false;
uniform sampler2D texture_normal;

struct pointLight{
	vec3 position;
	vec3 diffuse;
	float ambientFactor;
	float specularFactor;
	float range;
};
struct spotLight{
	vec3 position;
	vec3 direction;
	vec3 color;
	float outerCutoff, cutoff;
	float range;
};
struct dirLight{
	vec3 direction;
	vec3 color;
	float specularFactor, ambientFactor;
};
uniform vec3 viewPos;
uniform int pointLightNum = 0;
uniform pointLight lights[MAX_POINT_LIGHTS];
uniform int spotLightNum = 0;
uniform spotLight spots[MAX_POINT_LIGHTS];
uniform bool useDirectional = false;
uniform dirLight directional;
uniform float shininess;
vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor);
vec3 calcSpotLight(spotLight spot, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor);
vec3 calcDirLight(dirLight light, vec3 normal, vec3 viewDir, vec3 fragColor, vec3 specColor);
void main(){
	vec4 FragColor = vec4(vec3(0.0), 1.0);
	vec3 norm = Normal;
	if(color.r < -0.2)
		FragColor = texture(texture_diffuse, Tex_Coords);
	else
		FragColor = color;
	if(pointLightNum == 0 && spotLightNum == 0)
		fragColor = FragColor;
	if(useNormalMap){
		norm = normalize(normalize(texture(texture_normal, Tex_Coords).rgb) * 2.0 - 1.0);
		vec3 tan = Tan_To_World[0];
		vec3 bit = Tan_To_World[1];
		vec3 calc = cross(tan, bit);
		if(dot(calc, Tan_To_World[2]) < 0){ //if flipped
			tan = -tan;
			mat3 tbn = Tan_To_World;
			tbn[0] = tan;
			norm = normalize(tbn * norm);
		}
		else
			norm = normalize(Tan_To_World * norm);
	}
	vec3 specColor = useSpecMap ? texture(texture_specular, Tex_Coords).rgb : vec3(1);
	for(int i = 0; i < pointLightNum; ++i){		
		fragColor.rgb += calcPointLight(lights[i], norm, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	}
	for(int i = 0; i < spotLightNum; ++i){
		fragColor.rgb += calcSpotLight(spots[i], norm, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	}
	if(useDirectional)
		fragColor.rgb += calcDirLight(directional, norm, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor){
	vec3 lightDirection = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDir = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	float dist = length(light.position - fragPos);
	float attenuation = light.range / (dist * dist);

	vec3 ambient = light.ambientFactor * fragColor * light.diffuse;
	vec3 diffuse = diff * light.diffuse * fragColor;
	vec3 specular = light.specularFactor * spec * specColor * light.diffuse;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}
vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor){
	vec3 ambient = 0.2 * fragColor * light.color;
		
	//diffuse
	vec3 lightDirection = normalize(light.position - fragPos); 
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diff * fragColor;

	//specular
	vec3 reflectDirection = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0), shininess);
	vec3 specular = spec * light.color * specColor;

	//soft edges - spotlight
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = (light.cutoff - light.outerCutoff);
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
	ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	//attenuation
	float dist = length(light.position - fragPos);
	float attenuation =  light.range / (dist * dist);
	diffuse *= attenuation;
	specular *= attenuation;
	ambient *= attenuation;

	return (ambient + diffuse + specular);
}
vec3 calcDirLight(dirLight light, vec3 normal, vec3 viewDir, vec3 fragColor, vec3 specColor) {
	vec3 lightDirection = normalize(light.direction);

	float diff = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDir = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = light.ambientFactor * fragColor;
	vec3 diffuse = light.color * diff * fragColor;
	vec3 specular = light.specularFactor * spec * light.color * specColor;
	return (ambient + diffuse + specular);
}�      �����     0 	        #version 330 core
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

}�       �����     0 	        #version 330 core

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D texture_diffuse;

void main() {
	frag_color = texture(texture_diffuse, tex_coords);
}   �       �����     0 	        #version 330 core
layout (location = 0) in vec3 pos;
out vec3 texCoords;
uniform mat4 projection;
uniform mat4 view;
void main(){
	texCoords = pos;
	gl_Position = projection * view * vec4(pos, 1.0);
}
  �       �����     0 	        #version 330 core
out vec4 FragColor;
in vec3 texCoords;
uniform samplerCube texture_skybox;
void main(){
	FragColor = texture(texture_skybox, texCoords);
}
