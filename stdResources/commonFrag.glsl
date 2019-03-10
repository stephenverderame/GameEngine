#version 330 core
#define MAX_POINT_LIGHTS 128

in vec2 Tex_Coords;
in vec3 Normal;
in vec3 Frag_Pos;

out vec4 fragColor;

uniform vec4 color = {-1.0, -1.0, -1.0, 1.0};
uniform sampler2D texture_diffuse;
uniform bool useSpecMap;
uniform sampler2D texture_specular;

struct pointLight{
	vec3 position;
	vec3 diffuse;
	float ambientFactor;
	float specularFactor;
	float range;
};
uniform vec3 viewPos;
uniform int pointLightNum = 0;
uniform pointLight lights[MAX_POINT_LIGHTS];
uniform float shininess;
vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor);
void main(){
	vec4 FragColor;
	if(color.r < -0.2)
		FragColor = texture(texture_diffuse, Tex_Coords);
	else
		FragColor = color;
	fragColor = FragColor;
	for(int i = 0; i < pointLightNum; ++i){
		vec3 specColor = useSpecMap ? texture(texture_specular, Tex_Coords).rgb : vec3(1);
		fragColor.rgb += calcPointLight(lights[i], Normal, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	}
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor){
	vec3 lightDirection = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDir = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	float dist = length(light.position - fragPos);
	float attenuation = light.range / (dist * dist);

	vec3 ambient = light.ambientFactor * fragColor * light.diffuse;
	vec3 diffuse = diff * fragColor * light.diffuse;
	vec3 specular = light.specularFactor * spec * specColor * light.diffuse;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}