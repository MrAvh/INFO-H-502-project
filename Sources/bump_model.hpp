#include <string>

#ifndef BUMPMODEL_H
#define BUMPMODEL_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Shader.hpp"
#include "bump_mesh.hpp"
#include "spotLight.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

/*
	Intermediate class for meshes using bump mapping
	Posseses one shared mesh and own model to create only one mesh to be moved at each drawing
*/
class BumpModel {

	BumpMeshObject mesh;
	glm::mat4 model = glm::mat4(1.0f);

public:

	BumpModel();
	BumpModel(BumpMeshObject mesh, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0));
	void rotate(glm::mat3 rotation);
	void translate(glm::vec3 translation);
	void draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light);
	void reflectionDraw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture);

};
#endif