#include <string>

#ifndef ROAD_H
#define ROAD_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "bump_mesh.hpp"
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
	Road class
	Contains and handles the road elements, sources of light and collisions
	Is generated from a .txt file for each subitem placement
*/
class Road{

	glm::vec3 position = glm::vec3(0, 0, 0);
	std::vector <glm::vec3> light_positions;

	int CLASSIC_ROAD = 0;
	int CROSSING = 1;
	int LIGHT = 2;
	int BORDER = 3;

	struct RoadElement {
		Model model;
		glm::vec3 position;
		float angle;
		int type;
		RoadElement(Model model, glm::vec3 position, float angle, int type) {
			this->model = model;
			this->position = position;
			this->angle = angle;
			this->type = type;
		}
	};

	std::vector<RoadElement> road_elements;

public:
	Road();
	Road(char const* txt_file, int item_number, glm::vec3 initial_position);
	void draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light);
	std::vector<glm::vec3> getLightPositions();
	float detectVerticalCollisions(glm::vec3 direction, glm::vec3 initial_point);
	float detectHorizontalCollisions(glm::vec3 direction, glm::vec3 initial_point);

private:

	void createObjects(char const *txt_file, int item_number);
};

#endif