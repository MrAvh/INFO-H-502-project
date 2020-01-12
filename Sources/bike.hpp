#include <string>

#ifndef BIKE_H
#define BIKE_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "mesh.hpp"
#include "road.hpp"
#include "spotLight.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>

enum Direction {
	B_FORWARD,
	B_BACKWARD,
	B_BRAKE,
	B_LEFT,
	B_RIGHT
};

/*
	Bike class
	Handles all movements of the bike and its drawing
*/
class Bike {

	glm::vec3 position;
	glm::vec3 front;
	MeshObject body;
	MeshObject front_wheel;
	MeshObject rear_wheel;
	MeshObject exhaust;
	glm::vec3 rear_wheel_offset = glm::vec3(0, 0.34, -1.026);
	glm::vec3 front_wheel_offset = glm::vec3(0, 0.34, 0.667);
	glm::vec3 head_lamp_offset = glm::vec3(0, 0.95, 0.78);
	glm::vec3 exhaust_offset = glm::vec3(0, 0.09, 0);
	glm::vec3 exhaust_port_offset = glm::vec3(-0.24, 0.72, -1.04);
	glm::vec3 front_wheel_collision_point = glm::vec3(0, 0, 0.90);
	glm::vec3 rear_wheel_collision_point = glm::vec3(0, 0, -1.25);
	GLfloat velocity = 0.0;
	SpotLight head_lamp;

public:

	Bike();
	Bike(int item_number, glm::vec3 initial_position);
	void draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture, SpotLight light);
	void move(Road road, float delta_time);
	void accelerate(Direction direction, float delta_time);
	void turn(Direction direction, float delta_time, Road road);
	void reset();
	glm::vec3 getPosition();
	glm::vec3 getHeadLampPosition();
	glm::vec3 getExhaustPort();
	SpotLight getHeadLamp();

private:
	void wheelRotation();
	glm::vec3 processGravity(Road road, float delta_time);
	void processCollisions(Road road, float delta_time);
};
#endif