#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 color;
layout(location = 2) in vec3 normals;
out vec2 vertexColor;
out vec3 fragPos;
out vec3 Normal;
uniform mat4 MVP;
uniform mat4 model;
uniform vec3 lightPos;
void main(){
gl_Position =  MVP * vec4(position,1);
fragPos = vec3(model * vec4(position, 1));
vertexColor = vec2(color.x, 1.0-color.y);
Normal = mat3(transpose(inverse(model))) * normals;
}