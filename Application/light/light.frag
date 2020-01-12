#version 330 core
in vec2 vertexColor;
out vec4 color;
uniform sampler2D myTexture1;
void main(){
	color = texture(myTexture1, vertexColor);
}