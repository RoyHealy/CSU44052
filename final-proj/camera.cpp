#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// #include <stb/stb_image.h>
// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>

// #include "lab2_skybox.h"
#include "terrain.cpp"
// #include "buildings.cpp"

static GLFWwindow *window;
static int windowWidth = 1920, halfWidth = windowWidth/2;
static int windowHeight = 1024, halfHeight = windowHeight/2;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void cursor_callback(GLFWwindow *window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static float sensitivity = 1.5f;
// OpenGL camera view parameters
static float cameraSpeed = 2.f;
static glm::vec3 eye_center(0.0f, 70.0f, 0.0f);
static glm::vec3 lookdirection(1.0f, 0.0f, 0.0f);
static glm::vec3 up(0, 1, 0);

// View control 
static float viewAzimuth = 0.f;
static float viewPolar = M_PI_2;
// static float viewDistance = 300.0f;
static glm::float32 zNear = 0.1f, zFar = 5200.f, FoV = 45;



int main(void)
{
	// // Initialise GLFW
	// if (!glfwInit())
	// {
	// 	std::cerr << "Failed to initialize GLFW." << std::endl;
	// 	return -1;
	// }

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// // Open a window and create its OpenGL context
	// window = glfwCreateWindow(windowWidth, windowHeight, "Skybox", NULL, NULL);
	// if (window == NULL)
	// {
	// 	std::cerr << "Failed to open a GLFW window." << std::endl;
	// 	glfwTerminate();
	// 	return -1;
	// }
	// glfwMakeContextCurrent(window);

	// // Ensure we can capture the escape key being pressed below
	// glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// glfwSetKeyCallback(window, key_callback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// glfwSetCursorPosCallback(window, cursor_callback);

	// // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	// int version = gladLoadGL(glfwGetProcAddress);
	// if (version == 0)
	// {
	// 	std::cerr << "Failed to initialize OpenGL context." << std::endl;
	// 	return -1;
	// }

	// // Background
	// glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	// // TODO: Create more buildings
    // // ---------------------------
	// // std::vector<Building> buildings;
	
	// buildings.push_back(b);
    // ---------------------------
// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	GLFWwindow *window = glfwCreateWindow(1920, 1024, "proj", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_callback);
	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	std::string filename = "skyscraperB";
	std::string filepath = "../final-proj/models/" + filename + ".gltf";

	// Our 3D asset
    // MyAsset buildingA;
    // buildingA.initialize(filepath.c_str());
	Skybox box;
	box.initialize(glm::vec3(0, 0, 0), glm::vec3(3000, 3000, 3000));

	Terrain t;
	t.initialise();
	// std::vector<unsigned char> depth(201*201*4);
	// glGetTexImage(GL_TEXTURE_2D, t.getChunk(0,0), GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
	// // for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;

	// stbi_write_png("testing.png", 201, 201, 3, depth.data(), 201 * 3);
	// Camera setup
    glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time 
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;

	// Main loop
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		// Rendering
		viewMatrix = glm::lookAt(eye_center, eye_center+lookdirection, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;
		box.render(vp, eye_center);
		t.render(eye_center, vp);
		// FPS tracking 
		// Count number of frames over a few seconds and take average
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {		
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;
			
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Lab 4 | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

    box.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		eye_center = glm::vec3(0.0f, 0.0f, 0.0f);
		// lightPosition = glm::vec3(-275.0f, 500.0f, -275.0f);

	}
	glm::vec3 forwardSpeed = cameraSpeed*lookdirection;
	glm::vec3 sideSpeed = cameraSpeed*glm::cross(up, lookdirection);

	if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center += forwardSpeed;
	}

	if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center -= forwardSpeed;
	}

	if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center += sideSpeed;
	}

	if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center -= sideSpeed;
	}
	    
	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
    {
        eye_center.y += cameraSpeed;
    }
	if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.y -= cameraSpeed;
	}
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			cameraSpeed = 5.f;
		}
		if (action == GLFW_RELEASE) {
			cameraSpeed = 2.f;
		}
	}
	if (eye_center.y < 1) eye_center.y = 1;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static bool mouseOutsideView = true, LMB_HELD = false;
static void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
	if (xpos < 0 || xpos >= windowWidth || ypos < 0 || ypos > windowHeight || !focused) {
		mouseOutsideView = true;
		return;
	}	
	// Normalize to [0, 1] 
	float x = xpos / windowWidth;
	float y = ypos / windowHeight;

	// To [-1, 1] and flip y up 
	x = (x * 2.0f - 1.0f)*sensitivity;
	y = (1.0f - y * 2.0f)*sensitivity;

	if (mouseOutsideView) {
		x = 0; y = 0;
		mouseOutsideView = false;
	}

	viewPolar += -y;
	viewAzimuth += -x;
	if (viewPolar < 0) viewPolar += M_PI*2;
	if (viewPolar > M_PI*2) viewPolar -= M_PI*2;
	if (viewPolar < 0.1) viewPolar = 0.1;
	if (viewPolar > M_PI - 0.1) viewPolar = M_PI - 0.1;

	// glm::vec3 sideAxis = glm::normalize(glm::cross(up, lookdirection));
	lookdirection = glm::vec3(sin(viewPolar)*cos(viewAzimuth),cos(viewPolar),-sin(viewPolar)*sin(viewAzimuth));
	
	glfwSetCursorPos(window, halfWidth,halfHeight);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        if (action == GLFW_PRESS) 
			LMB_HELD = true;
		if (action == GLFW_RELEASE)
			LMB_HELD = false;
}