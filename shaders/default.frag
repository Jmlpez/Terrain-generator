#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

struct Material{
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

struct Light{
	vec3 position; 
	vec3 direction;
	// for the Spotlight
	float cutOffAngle;
	float outerCutOffAngle;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;	

	// for the attenuation
	float constant;
	float linear;
	float quadratic;

};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;
// Function to calculate directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {
	
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS]; //like C-array

// Function to calculate pointings lights
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight
{
	PointLight pointLight;	

	// for the Spotlight
	float cutOffAngle;
	float outerCutOffAngle;
};

uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPos,vec3 viewDir);


uniform Material material;
uniform Light light;

// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position of the fragment from the Vertex Shader
in vec3 FragPos;

// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
// uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

// uniform vec4 campPos

void main()
{

	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(camPos - FragPos);

	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	// phase 2: Point lights
	//for(int i = 0; i < NR_POINT_LIGHTS; i++){
	//	result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	//}
	// phase 3: Spot light
	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	// combine results
	// vec3 ambient = light.ambient * vec3(texture(material.diffuse0, texCoord));
	// vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse0, texCoord));
	// vec3 specular = light.specular * spec * vec3(texture(material.specular0,texCoord));
	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * diff * color;
	vec3 specular = light.specular * spec * color;

	return (ambient + diffuse + specular);

}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse0, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse0, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular0, texCoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);

}

vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPos,vec3 viewDir)
{
	//PointLight light;
	vec3 pointLightCalculation =  CalcPointLight(light.pointLight, normal, fragPos, viewDir);
	// vec3 x = dot(pepe, pepe);

	vec3 lightDir = normalize(light.pointLight.position - fragPos);

	float theta = dot(lightDir, normalize(-light.pointLight.direction));
	float epsilon = light.cutOffAngle - light.outerCutOffAngle;
	float intensity = clamp ((theta - light.outerCutOffAngle) / epsilon, 0.0, 1.0);

	pointLightCalculation *= intensity;

	return pointLightCalculation;	
}