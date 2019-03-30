#version 330 core
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
	vec3 position;
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
vec3 calcDirLight(dirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor);
void main(){
	vec4 FragColor;
	fragColor = vec4(vec3(0), 1.0);
	vec3 norm = Normal;
	if(color.r < -0.2)
		FragColor = texture(texture_diffuse, Tex_Coords);
	else
		FragColor = color;
	vec3 specColor = useSpecMap ? texture(texture_specular, Tex_Coords).rgb : vec3(-1);
	if(useNormalMap){
		norm = normalize(texture(texture_normal, Tex_Coords).rgb * 2.0 - 1.0);
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
	if(pointLightNum == 0 && spotLightNum == 0 && !useDirectional)
		fragColor = FragColor;
	else if(useDirectional)
		fragColor.rgb = calcDirLight(directional, norm, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	for(int i = 0; i < pointLightNum; ++i){		
		fragColor.rgb += calcPointLight(lights[i], norm, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	}
	for(int i = 0; i < spotLightNum; ++i){
		fragColor.rgb += calcSpotLight(spots[i], norm, Frag_Pos, normalize(viewPos - Frag_Pos), FragColor.rgb, specColor);
	}
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor){
	vec3 lightDirection = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDir = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(normal, reflectDir), 0.0), shininess);

	float dist = length(light.position - fragPos);
	float attenuation = light.range / (dist * dist);

	vec3 ambient = light.ambientFactor * light.diffuse * fragColor;
	vec3 diffuse = diff * light.diffuse;
	vec3 specular = (light.specularFactor * light.diffuse) * spec * (specColor == vec3(-1) ? vec3(1.0) : specColor);
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular) * fragColor;
}
vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor){
	vec3 ambient = 0.2 * fragColor * light.color;
		
	//diffuse
	vec3 lightDirection = normalize(light.position - fragPos); 
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diff;

	//specular
	vec3 reflectDirection = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(normal, reflectDirection), 0.0), shininess);
	vec3 specular = spec * light.color * (specColor == vec3(-1) ? vec3(1.0) : specColor);

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

	return (ambient + diffuse + specular) * fragColor;
}
vec3 calcDirLight(dirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 fragColor, vec3 specColor) {
	vec3 lightDirection = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDir = normalize(lightDirection + viewDir);//reflect(-lightDirection, normal);
	float spec = pow(max(dot(normal, reflectDir), 0.0), shininess);

	vec3 ambient = light.ambientFactor * fragColor;
	vec3 diffuse = light.color * diff;
	vec3 specular = (light.specularFactor * light.color) * spec;// * (specColor == vec3(-1) ? vec3(1.0) : specColor);
	return (ambient + diffuse + specular) * fragColor;
}