#version 330 core
in vec2 vertexColor;
in vec3 Normal;
in vec3 fragPos;
out vec4 color;
uniform int lightsCount;
uniform sampler2D myTexture1;
uniform vec3 lightPos[10];
uniform vec3 viewPos;
uniform samplerCube skybox;
void main(){
	color = texture(myTexture1, vertexColor);
	
	vec3 lightColor = vec3(1.0,1.0,1.0);
	
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos[0] - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	float distance = length(lightPos[0] - fragPos);
	diffuse = diffuse * (75.0 / (1.0 + (0.25 * distance * distance)));
	
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;	

    vec3 result = (ambient + diffuse + specular) * vec3(color.x,color.y,color.z);

	float mirrorEffect = 1.0;
	vec3 I = normalize(viewPos - fragPos);
	vec3 R = 2*dot(I,norm)*norm-I;
	vec3 reflection = texture(skybox, R).rgb;
	
	
    color = vec4(result*(1-mirrorEffect) + reflection*mirrorEffect, 1.0);
}