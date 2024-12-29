// #include <glad/gl.h>
// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtx/rotate_vector.hpp>
// #include <render/shader.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>

// #include <vector>
// #include <iostream>
// #define _USE_MATH_DEFINES
// #include <math.h>
// #include "lab2_skybox.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>

#include "lab2_skybox.h"
#include "terrain.cpp"

static GLFWwindow *window;
static int windowWidth = 1920, halfWidth = windowWidth/2;
static int windowHeight = 1024, halfHeight = windowHeight/2;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void cursor_callback(GLFWwindow *window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static float sensitivity = 2.f;
// OpenGL camera view parameters
static float cameraSpeed = 20.f;
static glm::vec3 eye_center(0.0f, 0.0f, 0.0f);
static glm::vec3 lookdirection(1.0f, 0.0f, 0.0f);
static glm::vec3 up(0, 1, 0);

// View control 
static float viewAzimuth = 0.f;
static float viewPolar = M_PI_2;
// static float viewDistance = 300.0f;
static glm::float32 zNear = 0.1f, zFar = 4000.f, FoV = 45;


static GLuint LoadTextureSkyBox(const char *texture_file_path) {
    int w, h, channels;
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  

    // // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // only this being GL_NEAREST seems to remove seams
	// unless i edit uv mappings to exclude a bit of the edge next to an unmapped pixel

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
    // Define the face sizes (assuming square faces)
}

struct Skybox {
	glm::vec3 position;		// Position of the box 
	glm::vec3 scale;		// Size of the box in each axis
	
	GLfloat vertex_buffer_data[72] = {	// Vertex definition for a canonical box
		// Front face
		1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 
		
		// Back face 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, -1.0f, 
		-1.0f, 1.0f, -1.0f, 
		-1.0f, 1.0f, 1.0f, 
		
		// Left face
		1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 

		// Right face 
		-1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 

		// Top face
		1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f, 

		// Bottom face
		1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, 1.0f, 
	};

	GLfloat color_buffer_data[72] = {
		// Front, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Back, yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		// Left, green
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Right, cyan
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 

		// Top, blue
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom, magenta
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 1.0f,  
	};

	GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
		0, 1, 2, 	
		0, 2, 3, 
		
		4, 5, 6, 
		4, 6, 7, 

		8, 9, 10, 
		8, 10, 11, 

		12, 13, 14, 
		12, 14, 15, 

		16, 17, 18, 
		16, 18, 19, 

		20, 21, 22, 
		20, 22, 23, 
	};

    // TODO: Define UV buffer data
    // --------------------------- 
		// X, red

		// Y, green

		// Z, blue
	GLfloat uv_buffer_data[48] = { // image is x across, y down
		// Front +X,
		0.0f, 0.665f, //2/3.0f,
		.25f, 0.665f, //2/3.0f,
		.25f, 0.334f, //1/3.0f,
		0.0f, 0.334f, //1/3.0f,
		// Back -X,
		0.5f, 0.665f,
		.75f, 0.665f,
		.75f, 0.334f,
		0.5f, 0.334f,
		// Left +Z,
		.25f, 2/3.0f,
		0.5f, 2/3.0f,
		0.5f, 1/3.0f,
		.25f, 1/3.0f,
		// Right -Z,
		.75f, 0.665f,
		1.0f, 0.665f,
		1.0f, 0.334f,
		.75f, 0.334f,
		// Top +Y,
		.251f, 1/3.0f,	// .25f, 1/3.0f,
		0.499f, 1/3.0f,	// 0.50f, 1/3.0f,
		0.499f, 0.0f,	// 0.50f, 0.0f,
		.251f, 0.0f,	// .25f, 0.0f,
		// Bottom -Y
		.251f, 1.0f,		    // .25f, 1.0f,
		0.499f, 1.0f,		// 0.50f, 1.0f,
		0.499f, 2/3.0f,		// 0.50f, 2/3.0f,
		.251f, 2/3.0f,		// .25f, 2/3.0f,
	};
    // ---------------------------

	// OpenGL buffers
	GLuint vertexArrayID; 
	GLuint vertexBufferID; 
	GLuint indexBufferID; 
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;

	void initialize(glm::vec3 position, glm::vec3 scale) {
		// Define scale of the building geometry
		this->position = position;
		this->scale = scale; // unneeded
		

		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data		
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
        // TODO: 
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		// scaled textures so it tiles building
		// for (int i = 0; i < 24; ++i) uv_buffer_data[2*i+1] *= 5;
		// TODO: Create a vertex buffer object to store the UV data
		// --------------------------------------------------------
		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);
        // --------------------------------------------------------
		// Create a vertex buffer object to store the UV data

		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../lab2/sbox.vert", "../lab2/sbox.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");

        // TODO: Load a texture 
        // --------------------
		textureID = LoadTextureSkyBox("../lab2/sky.png");
        // --------------------

        // TODO: Get a handle to texture sampler 
        // -------------------------------------
		// Get a handle for our "textureSampler" uniform
		textureSamplerID = glGetUniformLocation(programID,"textureSampler");
        // -------------------------------------

	}

	void render(glm::mat4 cameraMatrix, glm::vec3 pos) {
		this->position = pos;
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// TODO: Model transform 
		// -----------------------
        glm::mat4 modelMatrix = glm::mat4();    
        // Scale the box along each axis to make it look like a building
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, scale);
        // -----------------------

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// TODO: Enable UV buffer and texture sampler
		// ------------------------------------------
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// Set textureSampler to use texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0); 
		// ------------------------------------------

		// Draw the box
		glDrawElements(
			GL_TRIANGLES,      // mode
			36,    			   // number of indices
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
	}

	void cleanup() {
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &colorBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		glDeleteBuffers(1, &uvBufferID);
		glDeleteTextures(1, &textureID);
		glDeleteProgram(programID);
	}
}; 

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
	box.initialize(glm::vec3(0, 0, 0), glm::vec3(2000, 2000, 2000));

	Terrain t;
	t.initialise();

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
			cameraSpeed = 50.f;
		}
		if (action == GLFW_RELEASE) {
			cameraSpeed = 20.f;
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