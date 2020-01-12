#include "bike.hpp"


Bike::Bike() {}

Bike::Bike(int item_number, glm::vec3 initial_position) {
	/*
		Constructor
	*/
	position = initial_position;
	front = glm::vec3(0.0, 0.0, 1.0);
	Shader body_shader = Shader("bike/body.vert", "bike/body.frag", NULL, NULL, NULL);
	body = MeshObject("bike/smooth_body.obj", "bike/motorbike.jpg", item_number, body_shader, position);
	front_wheel = MeshObject("bike/front_wheel.obj", "bike/motorbike.jpg", item_number, body_shader, position + front_wheel_offset);
	rear_wheel = MeshObject("bike/rear_wheel.obj", "bike/motorbike.jpg", item_number, body_shader, position + rear_wheel_offset);
	exhaust = MeshObject("bike/exhaust.obj", "bike/motorbike.jpg", item_number, Shader("bike/exhaust.vert", "bike/exhaust.frag", NULL, NULL, NULL), position + exhaust_offset);
	head_lamp = SpotLight(position + head_lamp_offset, front, 0.5f, 0.7f);
}

void Bike::draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, GLuint skybox_texture, SpotLight light) {
	/*
		Draw all bike elements
	*/
	body.draw(vp, light_positions, camera_position, light);
	front_wheel.draw(vp, light_positions, camera_position, light);
	rear_wheel.draw(vp, light_positions, camera_position, light);
	exhaust.reflectionDraw(vp, light_positions, camera_position, skybox_texture);

}

void Bike::move(Road road, float delta_time) {
	/*
		Move the bike according to the velocity 
		after processing gravity and collisions
	*/
	glm::vec3 vertical_offset = processGravity(road, delta_time);
	processCollisions(road, delta_time);
	this->position += this->front * velocity + vertical_offset;
	body.translate(position);
	wheelRotation();
	front_wheel.translate(position + front_wheel_offset);
	rear_wheel.translate(position + rear_wheel_offset);
	exhaust.translate(position + exhaust_offset);
	head_lamp.setPosition(position + head_lamp_offset);
}

void Bike::accelerate(Direction direction, float delta_time) {
	/*
		Accelerates or brakes the bike according to 
		the direction and the actual speed
	*/
	if (velocity > 0.0) {
		if (direction == B_FORWARD)
			velocity = velocity + 0.8 * delta_time;
		else if (direction == B_BRAKE) {
			velocity = velocity - 1.5 * delta_time;
			if (velocity < 0.0)
				velocity = 0.0;
		}
	}
	else if (velocity == 0.0) {
		if (direction == B_FORWARD)
			velocity = velocity + 0.8 * delta_time;
		else if (direction == B_BACKWARD)
			velocity = velocity - 1.5 * delta_time;
	}
	else if (velocity > -0.01) {
		if (direction == B_FORWARD)
			velocity = 0.0;
		if (direction == B_BACKWARD)
			velocity = velocity - 0.1 * delta_time;
	}
	else {
		if (direction == B_FORWARD)
			velocity = 0.0;
	}
	if (velocity > 1.0) {
		velocity = 1.0;
	}
}

void Bike::turn(Direction direction, float delta_time, Road road) {
	/*
		Rotate all bike elements after processing potential collisions
	*/
	float theta;
	if (direction == B_LEFT)
		theta = 0.2 * delta_time;
	else if (direction == B_RIGHT)
		theta = -0.2 * delta_time;
	glm::mat3 rotation = glm::mat3(1.0f);
	rotation[0][0] = cos(theta);
	rotation[2][0] = sin(theta);
	rotation[0][2] = -sin(theta);
	rotation[2][2] = cos(theta);

	glm::vec3 front_displacement = rotation * front_wheel_collision_point - front_wheel_collision_point;
	glm::vec3 rear_displacement = rotation * rear_wheel_collision_point - rear_wheel_collision_point;

	float front_collision_time = road.detectHorizontalCollisions(front_displacement, position + front_wheel_collision_point);
	float rear_collision_time = road.detectHorizontalCollisions(rear_displacement, position + rear_wheel_collision_point);
	float collision_time = std::min(front_collision_time, rear_collision_time);

	if (collision_time < 1) {
		theta = 0;
		rotation[0][0] = cos(theta);
		rotation[2][0] = sin(theta);
		rotation[0][2] = -sin(theta);
		rotation[2][2] = cos(theta);
	}
	   	  
	front = rotation * front;
	body.rotate(rotation);
	front_wheel_offset = rotation * front_wheel_offset;
	front_wheel.rotate(rotation);
	rear_wheel_offset = rotation * rear_wheel_offset;
	rear_wheel.rotate(rotation);
	head_lamp_offset = rotation * head_lamp_offset;
	head_lamp.setPosition(position + head_lamp_offset);
	head_lamp.setDirection(front);
	exhaust_offset = rotation * exhaust_offset;
	exhaust.rotate(rotation);
	exhaust_port_offset = rotation * exhaust_port_offset;
	front_wheel_collision_point = rotation * front_wheel_collision_point;
	rear_wheel_collision_point = rotation * rear_wheel_collision_point;
}


glm::mat3 crossProductToMatrix(glm::vec3 A) {
	glm::mat3 matrix = glm::mat3(0.0f);
	matrix[0] = glm::vec3(0, A.z, -A.y);
	matrix[1] = glm::vec3(-A.z, 0, A.x);
	matrix[2] = glm::vec3(A.y, -A.x, 0);
	return matrix;
	}


glm::mat3 outerProduct(glm::vec3 A) {
	glm::mat3 matrix = glm::mat3(0.0f);
	matrix[0] = glm::vec3(A.x*A.x, A.x*A.y, A.x*A.z);
	matrix[1] = glm::vec3(A.y*A.x, A.y*A.y, A.y*A.z);
	matrix[2] = glm::vec3(A.z*A.x, A.z*A.y, A.z*A.z);
	return matrix;
}


void Bike::wheelRotation() {
	/*
		Makes the wheels rotate according to the 
		velocity using Rodriguez equation
	*/
	float radius = 0.34;
	float theta = velocity / radius;
	glm::vec3 normal = crossProductToMatrix(glm::vec3(0, 1, 0))*front;
	glm::mat3 rotation = glm::mat3(1)*cos(theta) + outerProduct(normal)*(1 - cos(theta)) + crossProductToMatrix(normal)*sin(theta);
	front_wheel.rotate(rotation);
	rear_wheel.rotate(rotation);
}

glm::vec3 Bike::getPosition() {
	return position;
}

glm::vec3 Bike::getHeadLampPosition() {
	return position + head_lamp_offset;
}

glm::vec3 Bike::getExhaustPort() {
	return position + exhaust_port_offset;
}

SpotLight Bike::getHeadLamp() {
	return head_lamp;
}

void Bike::reset() {
	/*
		Set all elements to their original position
		Stops the bike movement
	*/
	position = glm::vec3(0, 0, 0);
	front = glm::vec3(0.0, 0.0, 1.0);
	velocity = 0.0;
	glm::mat4 model = glm::mat4(1.0f);
	body.setModel(model);
	front_wheel_offset = glm::vec3(0, 0.34, 0.667);
	model[3] = glm::vec4(front_wheel_offset, 1);
	front_wheel.setModel(model);
	rear_wheel_offset = glm::vec3(0, 0.34, -1.026);
	model[3] = glm::vec4(rear_wheel_offset, 1);
	rear_wheel.setModel(model);
	head_lamp_offset = glm::vec3(0, 0.95, 0.78);
	head_lamp.setPosition(position + head_lamp_offset);
	head_lamp.setDirection(front);
	exhaust_offset = glm::vec3(0, 0.09, 0);
	model[3] = glm::vec4(exhaust_offset, 1);
	exhaust.setModel(model);
	exhaust_port_offset = glm::vec3(-0.24, 0.72, -1.04);
	front_wheel_collision_point = glm::vec3(0, 0, 0.90);
	rear_wheel_collision_point = glm::vec3(0, 0, -1.25);

}

glm::vec3 Bike::processGravity(Road road, float delta_time) {
	/*
		Check for collisions with the road on vertical axis, according to gravity
	*/
	glm::vec3 gravity = glm::vec3(0, -9.81, 0);
	glm::vec3 vertical_offset = glm::vec3(0);
	float collisionTime = road.detectVerticalCollisions(gravity, position);
	if (collisionTime > 0) {
		if (collisionTime <= delta_time) {
			vertical_offset = gravity * collisionTime;
		}
		else if (collisionTime > delta_time) {
			vertical_offset = gravity * delta_time;
		}
	}
	return vertical_offset;
}

void Bike::processCollisions(Road road, float delta_time) {
	/*
		Calculates the maximum displacement allowed for the bike on front direction
		Stops the bike if collision is detected
	*/
	glm::vec3 direction = velocity * front;
	glm::vec3 collision_point;
	if (velocity >= 0) 
		collision_point = position + front_wheel_collision_point;
	else 
		collision_point = position + rear_wheel_collision_point;

	float collisionTime = road.detectHorizontalCollisions(direction, collision_point);
	if (collisionTime < 1) {
		this->position += this->front * velocity * collisionTime;
		velocity = 0;
	}
	
}