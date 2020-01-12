#include "bump_mesh.hpp"


BumpMeshObject::BumpMeshObject(){}

BumpMeshObject::BumpMeshObject(char const *obj_file, char const *texture_file, char const *normal_file, int num, Shader shader, glm::vec3 position) {
	/*
		Constructor
		Creates objects from different input files
	*/
	texture_num = num;
	num++;
	normal_num = num;
	num++;
	this->shader = shader;
	this->shader.compile();
	texture = createTexture(texture_file);
	normal_map = createTexture(normal_file);
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	std::vector< glm::vec3 > tangents;
	std::vector< glm::vec3 > bitangents;

	readObjFile(obj_file, vertices, uvs, normals);
	processTangents(vertices, uvs, tangents, bitangents);
	VAO = createVAO(vertices, uvs, normals, tangents, bitangents);

	vertices_count = vertices.size();

	model[3] = glm::vec4(position, 1);
}

GLuint BumpMeshObject::createTexture(char const *filename) {
	/*
		Generate the texture to be reused later
	*/
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Set texture filtering
	int width, height, n;
	unsigned char* image = stbi_load(filename, &width, &height, &n, 3); // load jpg, png, … in memory
	// send the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // generate the mipmaps
	stbi_image_free(image); // we don't need anymore the image data
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidently mess up our texture.
	return texture;
}

bool BumpMeshObject::readObjFile(char const *path, std::vector < glm::vec3 > & out_vertices, std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals) {
	/*
		Create mesh from .obj file by parsing it
	*/
	std::vector< unsigned int > vertex_indices, uv_indices, normal_indices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	std::ifstream infile(path);
	std::string line;
	while (std::getline(infile, line)) {
		std::stringstream linestream(line);
		std::string index;

		if (line.at(0) == 'v') {
			if (line.at(1) == ' ') {
				glm::vec3 vertex;
				linestream >> index >> vertex.x >> vertex.y >> vertex.z;
				temp_vertices.push_back(vertex);
			}
			else if (line.at(1) == 't') {
				glm::vec2 uv;
				linestream >> index >> uv.x >> uv.y;
				temp_uvs.push_back(uv);
			}
			else if (line.at(1) == 'n') {
				glm::vec3 normal;
				linestream >> index >> normal.x >> normal.y >> normal.z;
				temp_normals.push_back(normal);
			}
		}
		else if (line.at(0) == 'f') {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			std::string s = linestream.str();
			std::replace(s.begin(), s.end(), '/', ' ');
			linestream.str(s);
			linestream >> index >> vertexIndex[0] >> uvIndex[0] >> normalIndex[0] >> vertexIndex[1] >> uvIndex[1] >> normalIndex[1] >> vertexIndex[2] >> uvIndex[2] >> normalIndex[2];
			vertex_indices.push_back(vertexIndex[0]);
			vertex_indices.push_back(vertexIndex[1]);
			vertex_indices.push_back(vertexIndex[2]);
			uv_indices.push_back(uvIndex[0]);
			uv_indices.push_back(uvIndex[1]);
			uv_indices.push_back(uvIndex[2]);
			normal_indices.push_back(normalIndex[0]);
			normal_indices.push_back(normalIndex[1]);
			normal_indices.push_back(normalIndex[2]);
		}

	}

	for (unsigned int i = 0; i < vertex_indices.size(); i++) {
		unsigned int vertex_index = vertex_indices[i];
		glm::vec3 vertex = temp_vertices[vertex_index - 1];
		out_vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < uv_indices.size(); i++) {
		unsigned int uv_index = uv_indices[i];
		glm::vec2 uv = temp_uvs[uv_index - 1];
		out_uvs.push_back(uv);
	}
	for (unsigned int i = 0; i < normal_indices.size(); i++) {
		unsigned int normal_index = normal_indices[i];
		glm::vec3 normal = temp_normals[normal_index - 1];
		out_normals.push_back(normal);
	}

	return true;
}

void BumpMeshObject::processTangents(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, 
	std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents) {
	/*
		Process tangents and bitangents of vertices to be used in normal mapping
	*/
	for (unsigned int i = 0; i < vertices.size()/3; i++) {
		glm::vec3 edge1 = vertices[i*3 + 1] - vertices[i*3];
		glm::vec3 edge2 = vertices[i * 3 + 2] - vertices[i * 3];
		glm::vec2 deltaUV1 = uvs[i * 3 + 1] - uvs[i * 3];
		glm::vec2 deltaUV2 = uvs[i * 3 + 2] - uvs[i * 3];

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent = glm::vec3(0);
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		glm::vec3 bitangent = glm::vec3(0);
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		tangents.push_back(tangent);
		bitangents.push_back(bitangent);

	}
}

GLuint BumpMeshObject::createVAO(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, 
	std::vector<glm::vec3> &normals, std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents) {
	/*
		Create VAO to be used later
	*/
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);

	std::vector<float> data;

	for (int i = 0; i < vertices.size(); i += 1) {
		data.push_back(vertices[i].x);
		data.push_back(vertices[i].y);
		data.push_back(vertices[i].z);
		data.push_back(uvs[i].x);
		data.push_back(uvs[i].y);
		data.push_back(normals[i].x);
		data.push_back(normals[i].y);
		data.push_back(normals[i].z);
		int j = int(i / 3);
		data.push_back(tangents[j].x);
		data.push_back(tangents[j].y);
		data.push_back(tangents[j].z);
		data.push_back(bitangents[j].x);
		data.push_back(bitangents[j].y);
		data.push_back(bitangents[j].z);
	}

	// Bind vertex Array Object
	glBindVertexArray(VAO);

	// Copy vertices array in a buffer for OpenGL
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

	// Set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);

	// Unbind VAO
	glBindVertexArray(0);

	return VAO;
}


void BumpMeshObject::rotate(glm::mat3 rotation) {
	/*
		Rotate the model
	*/
	model = glm::mat4(rotation) * model;
}


void BumpMeshObject::translate(glm::vec3 translation) {
	/*
		Translate the model
	*/
	model[3] = glm::vec4(translation, model[3][3]);
}


void BumpMeshObject::setModel(glm::mat4 model) {
	this->model = model;
}


void BumpMeshObject::draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light) {
	/*
		Draw the object with its texture and lighting
	*/
	glm::mat4 mvp = vp * model;
	shader.use();
	shader.setVector3f("lightPos[0]", light_positions[0]);
	shader.setVector3f("lightPos[1]", light_positions[1]);
	shader.setVector3f("lightPos[2]", light_positions[2]);
	shader.setVector3f("lightPos[3]", light_positions[3]);
	shader.setVector3f("lightPos[4]", light_positions[4]);
	shader.setVector3f("lightPos[5]", light_positions[5]);
	shader.setVector3f("lightPos[6]", light_positions[6]);
	shader.setVector3f("lightPos[7]", light_positions[7]);
	shader.setVector3f("lightPos[8]", light_positions[8]);
	shader.setVector3f("lightPos[9]", light_positions[9]);
	shader.setVector3f("viewPos", camera_position);
	shader.setMatrix4("model", model);
	shader.setMatrix4("MVP", mvp);
	shader.setVector3f("spotLight.position", light.getPosition());
	shader.setVector3f("spotLight.direction", light.getDirection());
	shader.setFloat("spotLight.cutOff", cos(light.getCutOff()));
	shader.setFloat("spotLight.outerCutOff", cos(light.getOuterCutOff()));
	glActiveTexture(GL_TEXTURE0 + texture_num);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInteger("myTexture1", texture_num);
	glActiveTexture(GL_TEXTURE0 + normal_num);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	shader.setInteger("normalMap", normal_num);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices_count);
	glBindVertexArray(0);
}


void BumpMeshObject::reflectionDraw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture) {
	/*
		Draw the object with the reflection of the skybox
	*/
	glm::mat4 mvp = vp * model;
	shader.use();
	shader.setVector3f("viewPos", camera_position);
	shader.setMatrix4("model", model);
	shader.setMatrix4("MVP", mvp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	shader.setInteger("skybox", 0);
	glActiveTexture(GL_TEXTURE0 + texture_num);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInteger("myTexture1", texture_num);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices_count);
	glBindVertexArray(0);
}
