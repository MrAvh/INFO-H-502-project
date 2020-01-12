#include "bump_model.hpp"


BumpModel::BumpModel(){}

BumpModel::BumpModel(BumpMeshObject mesh, glm::vec3 position) {
	this->mesh = mesh;
	model[3] = glm::vec4(position, 1);
}


void BumpModel::rotate(glm::mat3 rotation) {
	/*
		Rotate the model
	*/
	model = glm::mat4(rotation) * model;
}


void BumpModel::translate(glm::vec3 translation) {
	/*
		Translate the model
	*/
	model[3] = glm::vec4(translation, model[3][3]);
}


void BumpModel::draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light) {
	/*
		Draw the mesh by first changing its model
	*/
	mesh.setModel(model);
	mesh.draw(vp, light_positions, camera_position, light);
}


void BumpModel::reflectionDraw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture) {
	/*
		Draw the mesh with reflection by first changing its model
	*/
	mesh.setModel(model);
	mesh.reflectionDraw(vp, light_positions, camera_position, skybox_texture);
}
