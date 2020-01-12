#include <string>

#ifndef WALL_H
#define WALL_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "mesh.hpp"
#include "bump_mesh.hpp"
#include "bump_model.hpp"
#include "model.hpp"
#include "spotLight.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <glm/glm.hpp>


/*
	House class
	Contains and handles the house elements
	Is generated from a .txt file for each subitem placement
*/
class House{

	glm::vec3 position = glm::vec3(0, 0, 0);
	std::vector <BumpModel> bump_elements;
	std::vector <Model> elements;

public:
	House();
	House(char const* txt_file, int item_number, glm::vec3 initial_position, float angle);
	void draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light);

private:
	void createObjects(char const *txt_file, int item_number, float main_angle);
};

#endif