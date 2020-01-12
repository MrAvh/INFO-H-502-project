#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 color;
layout(location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec3 bitangents;
out vec2 vertexColor;
out vec3 fragPos;
out mat3 TBN;
uniform mat4 MVP;
uniform mat4 model;
void main(){
	gl_Position =  MVP * vec4(position,1);
	fragPos = vec3(model * vec4(position, 1));
	vertexColor = vec2(color.x, 1.0-color.y);
	vec3 T = normalize(vec3(transpose(inverse(model)) * vec4(tangents,   0.0)));
    vec3 B = normalize(vec3(transpose(inverse(model)) * vec4(bitangents, 0.0)));
    vec3 N = normalize(vec3(transpose(inverse(model)) * vec4(normals,    0.0)));
    TBN = mat3(T, B, N);
}