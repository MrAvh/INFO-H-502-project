#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform float skyboxSize;

void main(){
	float scale = float(1.0/skyboxSize);
	vec3 uvs = TexCoords * scale;
	color = texture(skybox, uvs);
}