#version 330 core
in vec2 vertexColor;
in vec3 Normal;
in vec3 fragPos;
out vec4 color;
uniform int lightsCount;
uniform sampler2D myTexture1;
uniform vec3 lightPos[10];
uniform vec3 viewPos;

void main(){
	color = texture(myTexture1, vertexColor);
	
	vec3 lightColor = vec3(1.0,1.0,1.0);
	
	float ambientStrength = 0.1;	
	float diffuseStrengh = 0.05;
	float specularStrength = 0.0;
    vec3 ambient = ambientStrength * lightColor;
	vec3 diffuse = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	
	for (int i = 0; i<10; i++){
	if (lightPos[i].x!=0.0 || lightPos[i].y!=0.0 || lightPos[i].z!=0.0){
		vec3 lightDir = normalize(lightPos[i] - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 temp_diffuse = diff * lightColor;
		float distance = length(lightPos[i] - fragPos);
		diffuse += diffuseStrengh * temp_diffuse * (75.0 / (1.0 + (0.25 * distance * distance)));
		
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular += specularStrength * spec * lightColor;
	}
	}

    vec3 result = (ambient + diffuse + specular) * vec3(color.x,color.y,color.z);
    color = vec4(result, 1.0);	
}