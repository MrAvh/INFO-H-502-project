#include <string>

#ifndef BUMPMESH_H
#define BUMPMESH_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Shader.hpp"
#include "spotLight.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

/*
	Mesh class of objects using bump mapping
*/
class BumpMeshObject {

	GLuint texture;
	GLuint normal_map;
	GLuint VAO;
	int vertices_count;
	int texture_num;
	int normal_num;
	glm::mat4 model = glm::mat4(1.0f);
	Shader shader;

public:

	BumpMeshObject();
	BumpMeshObject(char const *obj_file, char const *texture_file, char const *normal_file, int num, Shader shader, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0));
	void rotate(glm::mat3 rotation);
	void translate(glm::vec3 translation);
	void setModel(glm::mat4 model);
	void draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light);
	void reflectionDraw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture);

private:

	bool readObjFile(char const *path, std::vector < glm::vec3 > & out_vertices, std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals);
	void processTangents(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents);
	GLuint createVAO(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents);
	GLuint createTexture(char const *filename);
};
#endif