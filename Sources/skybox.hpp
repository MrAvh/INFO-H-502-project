#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Shader.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifndef SKYBOX_H
#define SKYBOX_H
#endif

/*
	Skybox class
*/
class Skybox {

	GLuint texture;
	GLuint VAO;
	float skybox_size;
	Shader shader = Shader("skybox/skybox.vert", "skybox/skybox.frag", NULL, NULL, NULL);

public:

	Skybox(float size, std::vector<std::string> textures);
	void draw(glm::mat4 vp, glm::vec3 camera_position);
	GLuint getTexture();
private:

	GLuint createSkybox(std::vector<std::string> textures);
	GLuint createSkyboxVAO(float distance);

};