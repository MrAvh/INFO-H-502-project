#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

out vec2 TexCoords;
out vec4 particleColor;

uniform mat4 MVP;
uniform mat3 rotation;
uniform vec3 offset;
uniform vec4 color;
void main(){
	float scale = 0.05f;
	TexCoords = vec2(uv.x, 1.0 - uv.y);
	particleColor = color;
	gl_Position = MVP * vec4((rotation * (vertex * scale)) + offset, 1.0);
}