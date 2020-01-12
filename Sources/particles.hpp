#include <string>

#ifndef PARTICLES_H
#define PARTICLES_H


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

/*
	Particles class
	Particles spawn at a specified location with a predefined speed
	They have a small random displacemnt to have better rendering
	Particles live for a defined amount of time before dying
*/
class Particles {

	GLuint VAO;
	GLuint texture;
	int particles_count;
	int last_used_particle;
	int texture_num;
	glm::mat4 model = glm::mat4(1.0f);
	Shader shader;

	struct Particle{
		glm::vec3 pos, speed;
		glm::vec4 color;
		float life;
		float camera_distance;
		Particle() : pos(0.0f), speed(0.0f), color(1.0f), life(0.0f), camera_distance(0.0f) {}
		bool operator<(Particle &that) {
			return this->camera_distance > that.camera_distance;
		}
	};

	std::vector<Particle> particles;
	GLuint nr_particles = 500;

public:

	Particles();
	Particles(char const *texture_file, int num, Shader shader, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0));
	void lifeCycle(float delta_time, glm::mat4 mvp, glm::vec3 spawn_point, glm::vec3 camera_position);

private:

	GLuint createVAO(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs);
	GLuint createTexture(char const *texture_file);
	int findUnusedParticle();
	void respawnParticle(Particle &particle, glm::vec3 position, glm::vec3 speed, glm::vec3 offset);
	void render(glm::mat4 mvp, glm::vec3 camera_position, glm::vec3 spawn_point);
	glm::mat3 alignToCamera(glm::vec3 camera_position, glm::vec3 particle_position);
};
#endif