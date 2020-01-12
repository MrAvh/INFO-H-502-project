#include "road.hpp"

Road::Road() {

}

Road::Road(char const* txt_file, int item_number, glm::vec3 initial_position) {
	/*
		Contructor
	*/
	position = initial_position;
	createObjects(txt_file, item_number);
}

void Road::createObjects(char const *txt_file, int item_number) {
	/*
		Creates all the subobjects from the read file
		Only one mesh is created for each type
	*/
	std::ifstream infile(txt_file);
	std::string line;
	Shader road_shader = Shader("road/road.vert", "road/road.frag", NULL, NULL, NULL);
	Shader lightpost_shader = Shader("road/lightpost.vert", "road/lightpost.frag", NULL, NULL, NULL);
	Shader lightbulb_shader = Shader("road/light.vert", "road/light.frag", NULL, NULL, NULL);

	MeshObject road = MeshObject("road/road.obj", "road/road.jpg", item_number, road_shader);
	MeshObject crossing = MeshObject("road/crossing.obj", "road/road.jpg", item_number, road_shader);
	MeshObject border = MeshObject("road/border.obj", "road/road.jpg", item_number, road_shader);
	MeshObject lightpost = MeshObject("road/lightpost.obj", "road/lightpost.jpg", item_number, lightpost_shader);
	MeshObject lightbulb = MeshObject("road/lightbulb.obj", "road/lightpost.jpg", item_number, lightbulb_shader);

	while (std::getline(infile, line)) {
		std::stringstream linestream(line);
		std::string index;
		if (line.at(0) == 'r') {
			glm::vec3 vertex;
			float angle;
			linestream >> index >> vertex.x >> vertex.y >> vertex.z >> angle;
			Model road_model = Model(road);
			glm::mat3 rotation = glm::mat3(1.0f);
			rotation[0][0] = cos(angle);
			rotation[2][0] = sin(angle);
			rotation[0][2] = -sin(angle);
			rotation[2][2] = cos(angle);
			road_model.rotate(rotation);
			road_model.translate(position + vertex);
			road_elements.push_back(RoadElement(road_model, position+vertex, angle, CLASSIC_ROAD));
		}
		else if (line.at(0) == 'c') {
			glm::vec3 vertex;
			float angle;
			linestream >> index >> vertex.x >> vertex.y >> vertex.z >> angle;
			Model crossing_model = Model(crossing);
			glm::mat3 rotation = glm::mat3(1.0f);
			rotation[0][0] = cos(angle);
			rotation[2][0] = sin(angle);
			rotation[0][2] = -sin(angle);
			rotation[2][2] = cos(angle);
			crossing_model.rotate(rotation);
			crossing_model.translate(position + vertex);
			road_elements.push_back(RoadElement(crossing_model, position + vertex, angle, CROSSING));
		}
		else if (line.at(0) == 'l') {
			glm::vec3 vertex;
			float angle;
			linestream >> index >> vertex.x >> vertex.y >> vertex.z >> angle;
			Model lightpost_model = Model(lightpost);
			Model lightbulb_model = Model(lightbulb);
			glm::mat3 rotation = glm::mat3(1.0f);
			rotation[0][0] = cos(angle);
			rotation[2][0] = sin(angle);
			rotation[0][2] = -sin(angle);
			rotation[2][2] = cos(angle);
			lightpost_model.rotate(rotation);
			lightbulb_model.rotate(rotation);
			lightpost_model.translate(position + vertex);
			lightbulb_model.translate(position + vertex + glm::vec3(0, 3, 0));
			road_elements.push_back(RoadElement(lightpost_model, position + vertex, angle, LIGHT));
			road_elements.push_back(RoadElement(lightbulb_model, position + vertex + glm::vec3(0, 3, 0), angle, LIGHT));
			light_positions.push_back(vertex + glm::vec3(0, 3, 0));
		}

		else if (line.at(0) == 'b') {
			glm::vec3 vertex;
			float angle;
			linestream >> index >> vertex.x >> vertex.y >> vertex.z >> angle;
			Model border_model = Model(border);
			glm::mat3 rotation = glm::mat3(1.0f);
			rotation[0][0] = cos(angle);
			rotation[2][0] = sin(angle);
			rotation[0][2] = -sin(angle);
			rotation[2][2] = cos(angle);
			border_model.rotate(rotation);
			border_model.translate(position + vertex);
			road_elements.push_back(RoadElement(border_model, position + vertex, angle, BORDER));
		}
	}
}

void Road::draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light) {
	/*
		Draw all the elements considering light
	*/
	for (RoadElement element : road_elements) {
		element.model.draw(vp, light_positions, camera_position, light);
	}
	
}

std::vector<glm::vec3> Road::getLightPositions() {
	/*
		Return the positions of all lightbulbs
	*/
	return light_positions;

}


float Road::detectVerticalCollisions(glm::vec3 direction, glm::vec3 initial_point) {
	/*
		Detect vertical collisions with standard road and crossings
		Collisions are detected using intersection equation
		returns time to get to the surface
	*/
	float smallest_time = 1.0;
	for (RoadElement element : road_elements) {
		if (element.type == CLASSIC_ROAD) {
			glm::vec3 central_point = element.position;
			glm::vec3 N = glm::vec3(0, 1, 0);
			central_point.y = -central_point.y;
			float D = central_point.x * N.x + central_point.y * N.y + central_point.z * N.z;
			float numerator = -(N.x * initial_point.x + N.y * initial_point.y + N.z * initial_point.z + D);
			float denominator = N.x * direction.x + N.y * direction.y + N.z * direction.z;
			float time = numerator / denominator;
			if ((time < smallest_time) && (time >= 0)) {
				glm::vec3 intersection_point = initial_point + direction * time;
				float x_offset, z_offset;
				if (element.angle == 0) {
					x_offset = 2; z_offset = 4;
				}
				else {
					x_offset = 4; z_offset = 2;
				}
				if ((central_point.x - x_offset <= intersection_point.x) &&
					(intersection_point.x <= central_point.x + x_offset) &&
					(central_point.z - z_offset <= intersection_point.z) &&
					(intersection_point.z <= central_point.z + z_offset)) {
					smallest_time = time;
				}
			}
		}
		else if(element.type == CROSSING) {
			glm::vec3 central_point = element.position;
			glm::vec3 N = glm::vec3(0, 1, 0);
			central_point.y = -central_point.y;
			float D = central_point.x * N.x + central_point.y * N.y + central_point.z * N.z;
			float numerator = -(N.x * initial_point.x + N.y * initial_point.y + N.z * initial_point.z + D);
			float denominator = N.x * direction.x + N.y * direction.y + N.z * direction.z;
			float time = numerator / denominator;
			if ((time < smallest_time) && (time >= 0)) {
				glm::vec3 intersection_point = initial_point + direction * time;
				float x_offset = 4.45; 
				float z_offset = 4.45;
				if ((central_point.x - x_offset <= intersection_point.x) &&
					(intersection_point.x <= central_point.x + x_offset) &&
					(central_point.z - z_offset <= intersection_point.z) &&
					(intersection_point.z <= central_point.z + z_offset)) {
					smallest_time = time;
				}
			}
		}
	} 
	return smallest_time;
}



float Road::detectHorizontalCollisions(glm::vec3 direction, glm::vec3 initial_point) {
	/*
		Detect horizontal collisions
		Uses intervals verifications for detection by processing the initial and destination point. 
		If it crosses a border, returns the fraction of the displacement to get to this collision point
	*/
	float smallest_time = 1.0;
	for (RoadElement element : road_elements) {
		if (element.type == CLASSIC_ROAD) {
			float x_offset, z_offset;
			glm::vec3 central_point = element.position;
			if (element.angle == 0) {
				x_offset = 2; z_offset = 4;
			}
			else {
				x_offset = 4; z_offset = 2;
			}
			if ((central_point.x - x_offset <= initial_point.x) &&
				(initial_point.x <= central_point.x + x_offset) &&
				(central_point.z - z_offset <= initial_point.z) &&
				(initial_point.z <= central_point.z + z_offset)) {
				glm::vec3 destination_point = initial_point + direction;
				if (element.angle == 0) {
					if (destination_point.x > central_point.x + x_offset) {
						float distance = central_point.x + x_offset - initial_point.x;
						smallest_time = distance / direction.x;
					}
					else if (destination_point.x < central_point.x - x_offset) {
						float distance = central_point.x - x_offset - initial_point.x;
						smallest_time = distance / direction.x;
					}
				}
				else {
					if (destination_point.z > central_point.z + z_offset) {
						float distance = central_point.z + z_offset - initial_point.z;
						smallest_time = distance / direction.z;
					}
					else if (destination_point.z < central_point.z - z_offset) {
						float distance = central_point.z - z_offset - initial_point.z;
						smallest_time = distance / direction.z;
					}
				}
			}
		}
		else if (element.type == BORDER) {
			float x_offset, z_offset;
			glm::vec3 central_point = element.position;
			glm::vec3 destination_point = initial_point + direction;
			if (element.angle == 0) {
				x_offset = 0.15; z_offset = 4.225;
			}
			else {
				x_offset = 4.225; z_offset = 0.15;
			}
			if ((central_point.x - x_offset <= destination_point.x) &&
				(destination_point.x <= central_point.x + x_offset) &&
				(central_point.z - z_offset <= destination_point.z) &&
				(destination_point.z <= central_point.z + z_offset)) {
				if (element.angle == 0) {
					if (initial_point.x >= central_point.x + x_offset) {
						float distance = central_point.x + x_offset - initial_point.x;
						smallest_time = distance / direction.x;
					}
					else if (initial_point.x <= central_point.x - x_offset) {
						float distance = central_point.x - x_offset - initial_point.x;
						smallest_time = distance / direction.x;
					}
				}
				else {
					if (initial_point.z >= central_point.z + z_offset) {
						float distance = central_point.z + z_offset - initial_point.z;
						smallest_time = distance / direction.z;
					}
					else if (initial_point.z <= central_point.z - z_offset) {
						float distance = central_point.z - z_offset - initial_point.z;
						smallest_time = distance / direction.z;
					}
				}
			}
		}
	}
	return smallest_time;
}

