#include <string>

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
	spotLight class
*/
class SpotLight {
	glm::vec3 direction;
	glm::vec3 position;
	float cut_off;
	float outer_cut_off;

public:
	SpotLight(){}
	SpotLight(glm::vec3 position, glm::vec3 direction, float cut_off, float outer_cut_off) {
		this->direction = direction;
		this->position = position;
		this->cut_off = cut_off;
		this->outer_cut_off = outer_cut_off;
	}

	void update(glm::vec3 position, glm::vec3 direction) {
		this->direction = direction;
		this->position = position;
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	glm::vec3 getDirection() {
		return direction;
	}

	void setDirection(glm::vec3 direction) {
		this->direction = direction;
	}


	float getCutOff() {
		return cut_off;
	}

	float getOuterCutOff() {
		return outer_cut_off;
	}
};
#endif