#include "skybox.hpp"



Skybox::Skybox(float size, std::vector<std::string> textures) {
	/*
		Constructor
	*/
	VAO = createSkyboxVAO(size);
	texture = createSkybox(textures);
	skybox_size = size;
	shader.compile();
}

GLuint Skybox::createSkybox(std::vector<std::string> textures) {
	/*
		Create the texture for the skybox
	*/
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, n;
	unsigned char* image;
	for (GLuint i = 0; i < textures.size(); i++) {
		image = stbi_load(textures[i].c_str(), &width, &height, &n, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture;
}

GLuint Skybox::createSkyboxVAO(float distance) {
	/*
		Create the skybox with defined size
	*/
	GLfloat vertices[] = {
		-distance, distance, -distance, -distance, -distance, -distance, distance, -distance, -distance,
		distance, -distance, -distance, distance, distance, -distance, -distance, distance, -distance,

		-distance, -distance, distance, -distance, -distance, -distance, -distance, distance, -distance,
		-distance, distance, -distance, -distance, distance, distance, -distance, -distance, distance,

		distance, -distance, -distance, distance, -distance, distance, distance, distance, distance,
		distance, distance, distance, distance, distance, -distance, distance, -distance, -distance,

		-distance, -distance, distance, -distance, distance, distance, distance, distance, distance,
		distance, distance, distance, distance, -distance, distance, -distance, -distance, distance,

		-distance, distance, -distance, distance, distance, -distance, distance, distance, distance,
		distance, distance, distance, -distance, distance, distance, -distance, distance, -distance,

		-distance, -distance, -distance, -distance, -distance, distance, distance, -distance, -distance,
		distance, -distance, -distance, -distance, -distance, distance, distance, -distance, distance,
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);

	// Bind vertex Array Object
	glBindVertexArray(VAO);

	// Copy vertices array in a buffer for OpenGL
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	return VAO;

}

void Skybox::draw(glm::mat4 vp, glm::vec3 camera_position) {
	/*
		Draw the skybox
	*/
	shader.use();
	shader.setMatrix4("VP", vp);
	shader.setVector3f("center", camera_position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	shader.setInteger("skybox", 0);
	shader.setFloat("skyboxSize", skybox_size);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36); 
	glBindVertexArray(0);
}

GLuint Skybox::getTexture() {
	return texture;
}