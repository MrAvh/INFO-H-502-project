#include "house.hpp"

House::House() {

}

House::House(char const* txt_file, int item_number, glm::vec3 initial_position, float angle) {
	/*
		Constructor
	*/
	position = initial_position;
	createObjects(txt_file, item_number, angle);
}

void House::createObjects(char const *txt_file, int item_number, float main_angle) {
	/*
		Creates all the subobjects from the read file
		Only one mesh is created for each type
	*/
	std::ifstream infile(txt_file);
	std::string line;

	Shader wall_shader = Shader("house/wall.vert", "house/wall.frag", NULL, NULL, NULL);
	BumpMeshObject wall = BumpMeshObject("house/wall.obj", "house/wall.jpg", "house/normal_wall.jpg", item_number, wall_shader);
	BumpMeshObject left_triangle_wall = BumpMeshObject("house/left_triangle_wall.obj", "house/wall.jpg", "house/normal_wall.jpg", item_number, wall_shader);
	BumpMeshObject right_triangle_wall = BumpMeshObject("house/right_triangle_wall.obj", "house/wall.jpg", "house/normal_wall.jpg", item_number, wall_shader);

	Shader roof_shader = Shader("house/roof.vert", "house/roof.frag", NULL, NULL, NULL);
	MeshObject roof = MeshObject("house/roof.obj", "house/roof.png", item_number, roof_shader);
	Shader door_shader = Shader("house/door.vert", "house/door.frag", NULL, NULL, NULL);
	MeshObject door = MeshObject("house/door.obj", "house/door.jpg", item_number, door_shader);
	MeshObject window = MeshObject("house/window.obj", "house/window.jpg", item_number, door_shader);

	glm::mat3 main_rotation = glm::mat3(1.0f);
	main_rotation[0][0] = cos(main_angle);
	main_rotation[2][0] = sin(main_angle);
	main_rotation[0][2] = -sin(main_angle);
	main_rotation[2][2] = cos(main_angle);

	while (std::getline(infile, line)) {
		std::stringstream linestream(line);
		std::string index;

		if (line.at(0) != '#') {
			glm::vec3 vertex;
			float angle;
			linestream >> index >> vertex.x >> vertex.y >> vertex.z >> angle;
			glm::mat3 rotation = glm::mat3(1.0f);
			rotation[0][0] = cos(angle);
			rotation[2][0] = sin(angle);
			rotation[0][2] = -sin(angle);
			rotation[2][2] = cos(angle);
			vertex = main_rotation * vertex;


			if (line.at(0) == 'w') {
				BumpModel wall_model = BumpModel(wall);
				wall_model.rotate(rotation);
				wall_model.rotate(main_rotation);
				wall_model.translate(position + vertex);
				bump_elements.push_back(wall_model);
			}

			else if (line.at(0) == 'r') {
				Model roof_model = Model(roof);
				roof_model.rotate(rotation);
				roof_model.rotate(main_rotation);
				roof_model.translate(position + vertex);
				elements.push_back(roof_model);
			}

			else if (line.at(0) == 't') {
				if (line.at(1) == 'l') {
					BumpModel wall_model = BumpModel(left_triangle_wall);
					wall_model.rotate(rotation);
					wall_model.rotate(main_rotation);
					wall_model.translate(position + vertex);
					bump_elements.push_back(wall_model);
				}

				else if (line.at(1) == 'r') {
					BumpModel wall_model = BumpModel(right_triangle_wall);
					wall_model.rotate(rotation);
					wall_model.rotate(main_rotation);
					wall_model.translate(position + vertex);
					bump_elements.push_back(wall_model);
				}
			}

			else if (line.at(0) == 'd') {
				Model door_model = Model(door);
				door_model.rotate(rotation);
				door_model.rotate(main_rotation);
				door_model.translate(position + vertex);
				elements.push_back(door_model);
			}

			else if (line.at(0) == 'f') {
				Model window_model = Model(window);
				window_model.rotate(rotation);
				window_model.rotate(main_rotation);
				window_model.translate(position + vertex);
				elements.push_back(window_model);
			}
		}
	}
}

void House::draw(glm::mat4 vp, std::vector<glm::vec3> light_positions, glm::vec3 camera_position, SpotLight light) {
	/*
		Draw all the elements considering light
	*/
	for (int i = 0; i < bump_elements.size(); i++) {
		bump_elements[i].draw(vp, light_positions, camera_position, light);
	}

	for (int i = 0; i < elements.size(); i++) {
		elements[i].draw(vp, light_positions, camera_position, light);
	}
	
}


