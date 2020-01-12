#include "particles.hpp"



Particles::Particles(){}

Particles::Particles(char const *texture_file, int num, Shader shader, glm::vec3 position) {
	/*
		Constructor
		Creates one VAO to be used multiple times
		Initialize the particles list
	*/
	texture_num = num;
	num++;
	this->shader = shader;
	this->shader.compile();
	texture = createTexture(texture_file);
	std::vector< glm::vec3 > vertices = {glm::vec3(-0.5, -0.5, 0), glm::vec3(0.5, -0.5, 0), glm::vec3(-0.5, 0.5, 0), glm::vec3(0.5, -0.5, 0), glm::vec3(-0.5, 0.5, 0), glm::vec3(0.5, 0.5, 0) };
	std::vector< glm::vec2 > uvs = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1) };

	VAO = createVAO(vertices, uvs);

	model[3] = glm::vec4(position, 1);

	for (GLuint i = 0; i < nr_particles; i++) {
		particles.push_back(Particle());
	}
	last_used_particle = 0;
}

GLuint Particles::createTexture(char const *texture_file) {
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
	unsigned char* image = stbi_load(texture_file, &width, &height, &n, 4); // load jpg, png, … in memory
	// send the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // generate the mipmaps
	stbi_image_free(image); // we don't need anymore the image data
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidently mess up our texture.
	return texture;
}

GLuint Particles::createVAO(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs) {
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
	}

	// Bind vertex Array Object
	glBindVertexArray(VAO);

	// Copy vertices array in a buffer for OpenGL
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

	// Set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return VAO;
}

void Particles::lifeCycle(float deltaTime, glm::mat4 vp, glm::vec3 spawn_point, glm::vec3 camera_position) {
	/*
		Handles one life cycle of the particles
		Creates new ones on dead ones
		Renders the particles facing the camera around y axis
	*/
	
	GLuint nr_new_particles = 10;

	for (GLuint i = 0; i < nr_new_particles; i++) {
		int unused_particle = findUnusedParticle();
		respawnParticle(particles[unused_particle], spawn_point, glm::vec3(0.0, -2.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
	}

	for (GLuint i = 0; i < nr_particles; i++) {
		Particle &p = particles[i];
		p.life -= deltaTime;
		if (p.life > 0.0f) {
			glm::vec3 random = glm::vec3(((rand() % 100) -50) / 10000.0f, ((rand() % 100)-50) / 10000.0f, ((rand() % 100)-50) / 10000.0f);
			p.speed += random;
			p.pos -= p.speed * deltaTime;
			p.color.a -= deltaTime * 0.125;
			p.camera_distance = glm::length(p.pos - camera_position);
		}
		else {
			p.camera_distance = -1.0f;
		}
	}
	render(vp, camera_position, spawn_point);
}

int Particles::findUnusedParticle() {
	/*
		Find a spot in the particles list where a particle is dead
	*/
	for (int i = last_used_particle; i < nr_particles; i++) {
		if (particles[i].life <= 0) {
			last_used_particle = i;
			return i;
		}
	}

	for (int i = 0; i < last_used_particle; i++) {
		if (particles[i].life <= 0) {
			last_used_particle = i;
			return i;
		}
	}
	last_used_particle = 0;
	return 0;
}

void Particles::respawnParticle(Particle &particle, glm::vec3 position, glm::vec3 speed, glm::vec3 offset){
	/*
		Creates a new particle
	*/
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.pos = position + offset;
	particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.life = 3.0f;
	particle.speed = speed * 0.1f;
}


glm::mat3 Particles::alignToCamera(glm::vec3 camera_position, glm::vec3 particle_position) {
	/*
		Align the particle to face the camera around Y axis
	*/
	glm::vec3 camera = normalize(camera_position - particle_position);
	glm::vec3 normal = glm::vec3(0, 0, 1);
	glm::vec3 projection = glm::vec3(camera.x, 0, camera.z);
	float theta = acos((normal.x * projection.x + normal.y * projection.y + normal.z * projection.z) / (glm::length(normal) * glm::length(projection)));
	if (projection.x > 0) {
		theta = -theta;
	}
	glm::mat3 rotation = glm::mat3(cos(theta), 0, sin(theta), 0, 1, 0, -sin(theta), 0, cos(theta));
	return rotation;
}

void Particles::render(glm::mat4 vp, glm::vec3 camera_position, glm::vec3 spawn_point) {
	/*
		Render all alive particles
	*/
	std::sort(&particles[0], &particles[nr_particles-1]);
	glm::mat3 rotation = alignToCamera(camera_position, spawn_point);
	glm::mat4 mvp = vp * model;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (Particle particle : particles) {
		if (particle.life > 0.0f) {

			shader.use();
			shader.setVector3f("offset", particle.pos);
			shader.setVector4f("color", particle.color);
			shader.setMatrix4("MVP", mvp);
			shader.setMatrix3("rotation", rotation);
			glActiveTexture(GL_TEXTURE0 + texture_num);
			glBindTexture(GL_TEXTURE_2D, texture);
			shader.setInteger("sprite", texture_num);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
}




