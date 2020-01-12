// Local Headers
#include "mesh.hpp"
#include "skybox.hpp"
#include "Camera.hpp"
#include "bike.hpp"
#include "house.hpp"
#include "road.hpp"
#include "particles.hpp"
#include "spotLight.hpp"


// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// To use stb_image, add this in *one* C++ source file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(suppress : 4996)


// Callbacks
static bool keys[1024]; // is a key pressed or not ?
						// External static callback
						// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/);
static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset);

void showFPS(void);
void cameraFocus();
std::vector<glm::vec3> getClosestLightPositions(std::vector<glm::vec3> light_positions, int quantity);

float delta_time = 0.0f;
std::vector<glm::vec3> light_positions;
Camera camera = Camera(glm::vec3(0,1,10));
double camera_x_pos = NULL;
double camera_y_pos = NULL;

Bike bike;
Road roads;
glm::vec3 focus_position;
bool focus_locked = false;
int item_number = 1;

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(1777, 1000, "Bike in the city", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set the required callback functions
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	// Change Viewport
	int width, height;
	glfwGetFramebufferSize(mWindow, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(1);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	
	// Create all game objects
	bike = Bike(item_number, glm::vec3(0,0,0));

	std::vector<House> houses;
	houses.push_back(House("house/house.txt", item_number, glm::vec3(-7.3, 0, 1), 0.0)); 
	houses.push_back(House("house/house.txt", item_number, glm::vec3(-8.3, 0, 15), 0.0));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(-20, 0, 1.5), 1.57));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(8, 0, -18), -1.57));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(13, 0, 12), 3.14));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(12, 0, 2), 1.57));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(14, 0, 36), 3.14));
	houses.push_back(House("house/house.txt", item_number, glm::vec3(12, 0, 21), 3.14));
	roads = Road("road/road.txt", item_number, glm::vec3(0, 0, 0));
	MeshObject grass = MeshObject("grass/grass.obj", "grass/grass.jpg", item_number, Shader("grass/grass.vert", "grass/grass.frag", NULL, NULL, NULL), glm::vec3(0, -0.01, 0));

	Particles smoke = Particles("particles/smoke.png", item_number, Shader("particles/smoke.vert", "particles/smoke.frag", NULL, NULL, NULL));

	Skybox skybox = Skybox(30.0, { "skybox/posx.jpg", "skybox/negx.jpg", "skybox/posy.jpg", "skybox/negy.jpg", "skybox/posz.jpg", "skybox/negz.jpg" });
		
	
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();
		bike.move(roads, delta_time);
		cameraFocus();

		//Process lights
		light_positions.clear();
		for (glm::vec3 light_post : roads.getLightPositions()) {
			light_positions.push_back(light_post);
		}

		light_positions = getClosestLightPositions(light_positions, 10);
		SpotLight bike_spot_light = bike.getHeadLamp();


		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 vp = projection * view;

        // Background Fill Color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);

		skybox.draw(vp, camera.Position);

		glDepthMask(GL_TRUE);

		grass.draw(vp, light_positions, camera.Position, bike_spot_light);
		bike.draw(vp, light_positions, camera.Position, skybox.getTexture(), bike_spot_light);
		roads.draw(vp, light_positions, camera.Position, bike_spot_light);
		for (House house : houses) {
			house.draw(vp, light_positions, camera.Position, bike_spot_light);
		}
		
		smoke.lifeCycle(delta_time, vp, bike.getExhaustPort(), camera.Position);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}

void showFPS(void) {
	/*
		Calculate FPS and delta time of the running application
	*/
	static double time_accumulator = glfwGetTime();
	static double last_time = glfwGetTime();
	static int nb_frames = 0;

	// Measure speed
	double current_time = glfwGetTime();
	delta_time = current_time - last_time;
	last_time = current_time;
	nb_frames++;
	if (current_time - time_accumulator >= 1.0) { // If last prinf() was more than 1 sec ago
										 // printf and reset timer
		std::cout << 1000.0 / double(nb_frames) << " ms/frame -> " << nb_frames << " frames/sec" << std::endl;
		nb_frames = 0;
		time_accumulator += 1.0;
	}
}


static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/){
	/*
		Process Keyboard inputs
	*/
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

	// V-SYNC
	if (keys[GLFW_KEY_U]) {
		static bool vsync = true;
		if (vsync) {
			glfwSwapInterval(0);
		}
		else {
			glfwSwapInterval(1);
		}
		vsync = !vsync;
	}
	if (keys[GLFW_KEY_UP]) {
		camera.ProcessKeyboard(FORWARD, 1);
	}
	if (keys[GLFW_KEY_DOWN]) {
		camera.ProcessKeyboard(BACKWARD, 1);
	}
	if (keys[GLFW_KEY_LEFT]) {
		camera.ProcessKeyboard(LEFT, 1);
	}
	if (keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(RIGHT, 1);
	}
	if (keys[GLFW_KEY_I]) {
		bike.accelerate(B_FORWARD, delta_time);
	}
	if (keys[GLFW_KEY_K]) {
		bike.accelerate(B_BRAKE, delta_time);
	}
	if (keys[GLFW_KEY_O]) {
		bike.accelerate(B_BACKWARD, delta_time);
	}
	if (keys[GLFW_KEY_J]) {
		for (int i = 0; i< 20; i++)
			bike.turn(B_LEFT, 0.016, roads);
	}
	if (keys[GLFW_KEY_L]) {
		for (int i = 0; i < 20; i++)
			bike.turn(B_RIGHT, 0.016, roads);
	}
	if (keys[GLFW_KEY_B]) {
		std::cout << "Camera locked on the bike" << std::endl;
		focus_position = bike.getPosition();
		focus_locked = !focus_locked;

	}
	if (keys[GLFW_KEY_X]) {
		std::cout << "Bike reset" << std::endl;
		bike.reset();
	}

}

static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
	/*
		Process Mouse inputs
	*/
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_RIGHT] = true;
	else 
		keys[GLFW_MOUSE_BUTTON_RIGHT] = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_LEFT] = true;
	else
		keys[GLFW_MOUSE_BUTTON_LEFT] = false;

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = true;
	else
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	/*
		Process mouse movements
	*/
	if (keys[GLFW_MOUSE_BUTTON_RIGHT]) {
		if (camera_x_pos == NULL) {
			camera_x_pos = xpos;
			camera_y_pos = ypos;
		}
		double xoffset = xpos - camera_x_pos;
		double yoffset = camera_y_pos - ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);

		camera_x_pos = xpos;
		camera_y_pos = ypos;
	}
	else {
		camera_x_pos = NULL;
	}
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset){
	/*
		Process scrolling
	*/
	
}

void cameraFocus() {
	/*
		Lock the camera on the bike
		The camera will follow the bike but is still free of movement
	*/
	if (focus_locked) {
		glm::vec3 new_position = bike.getPosition();
		glm::vec3 delta = new_position - focus_position;
		focus_position = new_position;
		camera.Position = camera.Position + delta;
	}
}

std::vector<glm::vec3> getClosestLightPositions(std::vector<glm::vec3> light_positions, int quantity) {
	/*
		Return the desired quantity of lights closer to the camera
		Helps reduce the light processing
		input: lightPositions, quantity
	*/
	glm::vec3 position = camera.Position;
	std::vector<float> distances;
	for (int i = 0; i < light_positions.size(); ++i) {
		distances.push_back(glm::distance(position, light_positions[i]));
	}
	std::vector<glm::vec3> output;
	int counter = 0;
	while (counter < quantity && (distances.size() - counter) > 0) {
		int index = std::min_element(distances.begin(), distances.end()) - distances.begin();
		output.push_back(light_positions[index]);
		distances[index] = FLT_MAX;
		++counter;
	}

	while (output.size() < quantity) {
		output.push_back(glm::vec3(0, 0, 0));
	}

	return output;
}
