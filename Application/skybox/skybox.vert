#version 330 core
layout(location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 VP;
uniform vec3 center;

void main(){
	vec3 new_position = position + center;
	gl_Position = VP * vec4(new_position, 1.0);
	TexCoords = position;
}