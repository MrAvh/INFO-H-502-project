#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 color;
out vec2 vertexColor;
uniform mat4 MVP;
uniform mat4 model;
uniform vec3 lightPos;
void main(){
gl_Position =  MVP * vec4(position,1);
vertexColor = vec2(color.x, 1.0-color.y);
}