#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void cursor_callback(GLFWwindow *window, double xpos, double ypos);

// OpenGL camera view parameters
static glm::vec3 eye_center(-278.0f, 273.0f, 800.0f);
static glm::vec3 lookat(-278.0f, 273.0f, 0.0f);
static glm::vec3 up(0.0f, 1.0f, 0.0f);
static float FoV = 45.0f;
static float zNear = 600.0f;
static float zFar = 3000.0f; 

// Lighting control 
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
static glm::vec3 lightIntensity = 5.0f * (8.0f * wave500 + 15.6f * wave600 + 18.4f * wave700);
static glm::vec3 lightPosition(-275.0f, 500.0f, -275.0f);

// Shadow mapping
static glm::vec3 lightUp(0, 0, 1);
static int shadowMapWidth = 1024;
static int shadowMapHeight = 768;

// TODO: set these parameters 
static float depthFoV = 120.f;
static float depthNear = 10.f;//110.f;
static float depthFar = 1000.f; 

// gloabl variable
GLuint shadowMap;
// Helper flag and function to save depth maps for debugging
static bool saveDepth = true;

// This function retrieves and stores the depth map of the default frame buffer 
// or a particular frame buffer (indicated by FBO ID) to a PNG image.
static void saveDepthTexture(GLuint fbo, std::string filename) {
    int width = shadowMapWidth;
    int height = shadowMapHeight;
	if (shadowMapWidth == 0 || shadowMapHeight == 0) {
		width = windowWidth;
		height = windowHeight;
	}
    int channels = 3; 
    
    std::vector<float> depth(width * height*4);
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // glReadBuffer(GL_DEPTH_COMPONENT);
    // glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned char> img(width * height * 3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;

    stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}

static void calculateNormals(GLfloat* vertexBuffer, GLfloat normalBuffer[], int faces, bool outside) {
	for (int i = 0; i < faces; i++) {
		int group = i*12; // 4 vertices 3 coords
		glm::vec3 corners[4];
		
		for (int j = 0; j < 4; j++) {
			corners[j] = glm::vec3(vertexBuffer[group+(j*3)],
				vertexBuffer[group+(j*3)+1],
				vertexBuffer[group+(j*3)+2]
			);
		}
		
		for (int j = 0; j < 4; j++) {
			glm::vec3 v1 = corners[(j!=0)?(j-1):3]-corners[j], v2 = corners[(j+1)%4]-corners[j];
			
			glm::vec3 cross;
			if (outside)
				cross = glm::cross(v2, v1);
			else 
				cross = glm::cross(v1, v2);
			glm::vec3 normal = glm::normalize(cross);
			// printf("%d(%f, %f, %f) cross %d(%f, %f, %f) = (%f, %f, %f)\n",(j-1)%4, v1.x, v1.y, v1.z, (j+1)%4,v2.x, v2.y, v2.z, cross.x, cross.y, cross.z);
			
			normalBuffer[group + j*3] = normal.x;
			normalBuffer[group + j*3+1] = normal.y;
			normalBuffer[group + j*3+2] = normal.z;
		}
	}
}

static void replace(bool tall, GLfloat vertexbuffer[], GLfloat colorbuffer[]) {
	std::vector<GLfloat> vertex;
	if (tall) // idk memcopy for cpp
		vertex = {
		// top
		-423.0, 330.0, -247.0,
		-265.0, 330.0, -296.0,
		-314.0, 330.0, -456.0,
		-472.0, 330.0, -406.0,
		// left
		-423.0,   0.0, -247.0,
		-423.0, 330.0, -247.0,
		-472.0, 330.0, -406.0,
		-472.0,   0.0, -406.0,
		// front
		-265.0,   0.0, -296.0,
		-265.0, 330.0, -296.0,
		-423.0, 330.0, -247.0,
		-423.0,   0.0, -247.0,
		// right
		-314.0,   0.0, -456.0,
		-314.0, 330.0, -456.0,
		-265.0, 330.0, -296.0,
		-265.0,   0.0, -296.0,
		// back
		-472.0,   0.0, -406.0,
		-472.0, 330.0, -406.0,
		-314.0, 330.0, -456.0,
		-314.0,   0.0, -456.0};
	else 
		vertex = {// top
		-130.0, 165.0 , -65.0, 
		-82.0, 165.0, -225.0,
		-240.0, 165.0, -272.0,
		-290.0, 165.0, -114.0,
		// left
		-290.0,   0.0, -114.0,
		-290.0, 165.0, -114.0,
		-240.0, 165.0, -272.0,
		-240.0,   0.0, -272.0,
		// back
		-130.0,   0.0,  -65.0,
		-130.0, 165.0,  -65.0,
		-290.0, 165.0, -114.0,
		-290.0,   0.0, -114.0,
		// right
		-82.0,   0.0, -225.0,
		-82.0, 165.0, -225.0,
		-130.0, 165.0,  -65.0,
		-130.0,   0.0,  -65.0,
		// front
		-240.0,   0.0, -272.0,
		-240.0, 165.0, -272.0,
		-82.0, 165.0, -225.0,
		-82.0,   0.0, -225.0};
	for (int i = 0; i < vertex.size(); i++) {
		vertexbuffer[i] = vertex[i];
		colorbuffer[i] = 1.0f;
	}
}

struct CornellBox {

	// Refer to original Cornell Box data 
	// from https://www.graphics.cornell.edu/online/box/data.html

	GLfloat vertex_buffer_data[60] = {
		// Floor 
		-552.8, 0.0, 0.0,   
		0.0, 0.0,   0.0,
		0.0, 0.0, -559.2,
		-549.6, 0.0, -559.2,

		// Ceiling
		-556.0, 548.8, 0.0,   
		-556.0, 548.8, -559.2,
		0.0, 548.8, -559.2,
		0.0, 548.8,   0.0,

		// Left wall 
		-552.8,   0.0,   0.0, 
		-549.6,   0.0, -559.2,
		-556.0, 548.8, -559.2,
		-556.0, 548.8,   0.0,

		// Right wall 
		0.0,   0.0, -559.2,   
		0.0,   0.0,   0.0,
		0.0, 548.8,   0.0,
		0.0, 548.8, -559.2,

		// Back wall 
		-549.6,   0.0, -559.2, 
		0.0,   0.0, -559.2,
		0.0, 548.8, -559.2,
		-556.0, 548.8, -559.2
	};

	// TODO: set vertex normals properly
	GLfloat normal_buffer_data[60] = {
		// Floor 
		0.0, 1.0, 0.0,   
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		// Ceiling
		0.0, -1.0, 0.0,   
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,

		// Left wall 
		-1.0, 0.0, 0.0,   
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,

		// Right wall 
		1.0, 0.0, 0.0,   
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		// Back wall 
		0.0, 0.0, -1.0,   
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0
	};

	GLfloat color_buffer_data[60] = {  
		// Floor
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// Ceiling
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// left wall
		1.0f, 0.0f, 0.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Right wall
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 

		// Back wall
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f  
	};

	GLuint index_buffer_data[30] = {
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
	};

	// OpenGL buffers
	GLuint vertexArrayID; 
	GLuint vertexBufferID; 
	GLuint indexBufferID; 
	GLuint colorBufferID;
	GLuint normalBufferID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint lightPositionID;
	GLuint lightIntensityID;
	GLuint programID;

	GLuint lightMatrixID;
	GLuint shadowMapID;

	//shadow shader ids
	GLuint shaderProgramID, shaderZfarID, shaderMvpID;

	// status 0 = walls, 1 = tall, 2 = short
	void initialize(int status) {
		if (status > 0) replace(status==1, vertex_buffer_data, color_buffer_data);
		// set normal buffer correctly
		calculateNormals(vertex_buffer_data, normal_buffer_data, 5, true);
		for (int i = 0; i < 20; i++) {
			printf("%f, %f, %f,\n", normal_buffer_data[i*3], normal_buffer_data[i*3+1], normal_buffer_data[i*3+2]);
		}
		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data		
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);
		
		// Create a vertex buffer object to store the vertex normals		
		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../lab3/box.vert", "../lab3/box.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");

		shadowMapID = glGetUniformLocation(programID, "shadowMap");
		lightMatrixID = glGetUniformLocation(programID, "lightSpaceMatrix");
		
		shaderProgramID = LoadShadersFromFile("../lab3/shadow.vert", "../lab3/shadow.frag");
		if (shaderProgramID == 0)
		{
			std::cerr << "Failed to load shadow shaders." << std::endl;
		}
		shaderZfarID = glGetUniformLocation(shaderProgramID, "farPlane");
		shaderMvpID = glGetUniformLocation(shaderProgramID, "MVP");
	}

	void shadowRender(glm::mat4 lightMatrix) {
		glUseProgram(shaderProgramID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glUniformMatrix4fv(shaderMvpID, 1, GL_FALSE, &lightMatrix[0][0]);

		glUniform3fv(shaderZfarID, 1, &depthFar);

		glDrawElements(
			GL_TRIANGLES,      // mode
			30,    			   // number of indices
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
	}

	void render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Set light data 
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

		// shadow stuff
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(shadowMapID, 0);

		glUniformMatrix4fv(lightMatrixID, 1, GL_FALSE, &lightMatrix[0][0]);

		// Draw the box
		glDrawElements(
			GL_TRIANGLES,      // mode
			30,    			   // number of indices
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
		glDeleteBuffers(1, &normalBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		
		glDeleteTextures(1, &shadowMapID);

		glDeleteProgram(programID);

		glDeleteProgram(shaderProgramID);
	}
}; 


struct Block {
	glm::vec3 position;		// Position of the box 
	// glm::vec3 scale;		// Size of the box in each axis
	
	GLfloat vertex_buffer_data[60] = {	// Vertex definition for a canonical box
		// top
		-130.0, 165.0 , -65.0, 
		-82.0, 165.0, -225.0,
		-240.0, 165.0, -272.0,
		-290.0, 165.0, -114.0,
		// left
		-290.0,   0.0, -114.0,
		-290.0, 165.0, -114.0,
		-240.0, 165.0, -272.0,
		-240.0,   0.0, -272.0,
		// back
		-130.0,   0.0,  -65.0,
		-130.0, 165.0,  -65.0,
		-290.0, 165.0, -114.0,
		-290.0,   0.0, -114.0,
		// right
		-82.0,   0.0, -225.0,
		-82.0, 165.0, -225.0,
		-130.0, 165.0,  -65.0,
		-130.0,   0.0,  -65.0,
		// front
		-240.0,   0.0, -272.0,
		-240.0, 165.0, -272.0,
		-82.0, 165.0, -225.0,
		-82.0,   0.0, -225.0
	};

	GLfloat color_buffer_data[60] = {
		// top, red
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// left, yellow
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// back, green
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// right, cyan
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 

		// front, blue
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	GLuint index_buffer_data[30] = {		// 12 triangle faces of a box
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

		// 20, 21, 22, 
		// 20, 22, 23, 
	};

	GLfloat normal_buffer_data[60] = {
		// Floor 
		0.0, -1.0, 0.0,   
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,

		// Ceiling
		0.0, 1.0, 0.0,   
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		// Left wall 
		0.0, -1.0, 0.0,   
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,

		// Right wall 
		0.0, -1.0, 0.0,   
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,

		// Back wall 
		0.0, -1.0, 0.0,   
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0
	};

    // // TODO: Define UV buffer data
    // // ---------------------------
	// GLfloat uv_buffer_data[48] = {
	// 	// Front
	// 	0.0f, 1.0f,
	// 	1.0f, 1.0f,
	// 	1.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	// Back
	// 	0.0f, 1.0f,
	// 	1.0f, 1.0f,
	// 	1.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	// Left
	// 	0.0f, 1.0f,
	// 	1.0f, 1.0f,
	// 	1.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	// Right
	// 	0.0f, 1.0f,
	// 	1.0f, 1.0f,
	// 	1.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	// Top - we do not want texture the top
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	// Bottom - we do not want texture the bottom
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// 	0.0f, 0.0f,
	// };
    // // ---------------------------

	// OpenGL buffers
	GLuint vertexArrayID; 
	GLuint vertexBufferID; 
	GLuint indexBufferID; 
	GLuint colorBufferID;
	GLuint normalBufferID;
	// GLuint uvBufferID;
	// GLuint textureID;

	// Shader variable IDs
	GLuint mvpMatrixID;

	GLuint lightPositionID;
	GLuint lightIntensityID;
	// GLuint textureSamplerID;
	GLuint programID;

	GLuint lightMatrixID;
	GLuint shadowMapID;

	void initialize(bool tall) {
		// Define scale of the building geometry
		// this->position = position;
		// this->scale = scale;
		GLfloat tall_vertex_buffer[60] = {
			// top
			-423.0, 330.0, -247.0,
			-265.0, 330.0, -296.0,
			-314.0, 330.0, -456.0,
			-472.0, 330.0, -406.0,
			// left
			-423.0,   0.0, -247.0,
			-423.0, 330.0, -247.0,
			-472.0, 330.0, -406.0,
			-472.0,   0.0, -406.0,
			// front
			-265.0,   0.0, -296.0,
			-265.0, 330.0, -296.0,
			-423.0, 330.0, -247.0,
			-423.0,   0.0, -247.0,
			// right
			-314.0,   0.0, -456.0,
			-314.0, 330.0, -456.0,
			-265.0, 330.0, -296.0,
			-265.0,   0.0, -296.0,
			// back
			-472.0,   0.0, -406.0,
			-472.0, 330.0, -406.0,
			-314.0, 330.0, -456.0,
			-314.0,   0.0, -456.0
		};
		if (tall) {
			calculateNormals(tall_vertex_buffer, normal_buffer_data, 5, true);
		}
		else {
			calculateNormals(vertex_buffer_data, normal_buffer_data, 5, true);
		}
		for (int i = 0; i < 20; i++) {
			printf("%f, %f, %f,\n", normal_buffer_data[i*3], normal_buffer_data[i*3+1], normal_buffer_data[i*3+2]);
		}
		
		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data		
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		if (tall) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(tall_vertex_buffer), tall_vertex_buffer, GL_STATIC_DRAW);
		}
		else
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
        // TODO: 
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);
		// GLfloat temp_uv_buffer_data[48];
		// std::copy(std::begin(uv_buffer_data), std::end(uv_buffer_data), std::begin(temp_uv_buffer_data));
		// scaled textures so it tiles building

		// for (int i = 0; i < 24; ++i) temp_uv_buffer_data[2*i+1] *= scale.y/4;
		// for (int i = 0; i < 24; ++i) temp_uv_buffer_data[2*i] *= (i<16?scale.x:scale.z)/4;
		// // TODO: Create a vertex buffer object to store the UV data
		// // --------------------------------------------------------
		// glGenBuffers(1, &uvBufferID);
		// glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(temp_uv_buffer_data), temp_uv_buffer_data, GL_STATIC_DRAW);
        // // --------------------------------------------------------
		// // Create a vertex buffer object to store the UV data
		// Create a vertex buffer object to store the vertex normals		
		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);
		
		programID = LoadShadersFromFile("../lab3/box.vert", "../lab3/box.frag");
		// Create and compile our GLSL program from the shaders
		// programID = LoadShadersFromFile("../lab2/box.vert", "../lab2/box.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
        // TODO: Load a texture 
        // --------------------
		// textureID = LoadTextureTileBox(buildtype[std::rand()%6]);
        // --------------------

        // TODO: Get a handle to texture sampler 
        // -------------------------------------
		// Get a handle for our "textureSampler" uniform
		// textureSamplerID = glGetUniformLocation(programID,"textureSampler");
        // -------------------------------------
		shadowMapID = glGetUniformLocation(programID, "shadowMap");
		lightMatrixID = glGetUniformLocation(programID, "lightSpaceMatrix");
	}

	void render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// TODO: Model transform 
		// -----------------------
        glm::mat4 modelMatrix = glm::mat4();    
        // Scale the box along each axis to make it look like a building
		modelMatrix = glm::translate(modelMatrix, position);
        // modelMatrix = glm::scale(modelMatrix, scale);
        // -----------------------

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// TODO: Enable UV buffer and texture sampler
		// ------------------------------------------
		// glEnableVertexAttribArray(2);
		// glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// // Set textureSampler to use texture unit 0
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, textureID);
		// glUniform1i(textureSamplerID, 0); 
		// ------------------------------------------
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
		
		// shadow stuff
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(shadowMapID, 1);

		glUniformMatrix4fv(lightMatrixID, 1, GL_FALSE, &lightMatrix[0][0]);

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
		glDeleteBuffers(1, &normalBufferID);
		// glDeleteBuffers(1, &uvBufferID);
		glDeleteTextures(1, &shadowMapID);
		glDeleteProgram(programID);
	}
}; 


int main(void)
{
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
	// window = glfwCreateWindow(windowWidth, windowHeight, "Lab 3", NULL, NULL);
	window = glfwCreateWindow(windowWidth, windowHeight, "Lab 3", NULL, NULL);
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

	// Prepare shadow map size for shadow mapping. Usually this is the size of the window itself, but on some platforms like Mac this can be 2x the size of the window. Use glfwGetFramebufferSize to get the shadow map size properly. 
    glfwGetFramebufferSize(window, &shadowMapWidth, &shadowMapHeight);

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    // Create the classical Cornell Box
	CornellBox b, sb, tb;
	b.initialize(0);
	tb.initialize(1);
	sb.initialize(2);
	// Camera setup
    glm::mat4 viewMatrix, projectionMatrix, depthProjection, depthView;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);
	
	//######## generated from chatgpt 4o mini
	
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint shadowFBO;
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Attach the shadow map texture as the depth attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	// Make sure we don't write to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check framebuffer status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}
	//######### end of generated

	depthProjection = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth/shadowMapHeight, depthNear, depthFar);
	
	do
	{
		// Bind the shadow FBO
		// glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		depthView = depthProjection * glm::lookAt(lightPosition, lightPosition+glm::vec3(0,-1,0), lightUp); // lightPosition+glm::vec3(0,-1,0)
		// render with FBO
		b.shadowRender(depthView);
		sb.shadowRender(depthView);
		tb.shadowRender(depthView);

		if (saveDepth) {
            std::string filename = "depth_camera.png";
            saveDepthTexture(shadowFBO, filename);
            std::cout << "Depth texture saved to " << filename << std::endl;
            saveDepth = false;
        }
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);   
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;
		
		b.render(vp, depthView);
		sb.render(vp, depthView);
		tb.render(vp, depthView);

		// Swap buffers
		glfwSwapBuffers(window);

		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	glDeleteFramebuffers(1, &shadowFBO);
	// Clean up
	b.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		eye_center = glm::vec3(-278.0f, 273.0f, 800.0f);
		lightPosition = glm::vec3(-275.0f, 500.0f, -275.0f);

	}

	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.y += 20.0f;
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.y -= 20.0f;
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x -= 20.0f;
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x += 20.0f;
	}

	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		lightPosition.z -= 20.0f;
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		lightPosition.z += 20.0f;
	}
	    
	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
    {
        saveDepth = true;
    }

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
	if (xpos < 0 || xpos >= windowWidth || ypos < 0 || ypos > windowHeight) 
		return;

	// Normalize to [0, 1] 
	float x = xpos / windowWidth;
	float y = ypos / windowHeight;

	// To [-1, 1] and flip y up 
	x = x * 2.0f - 1.0f;
	y = 1.0f - y * 2.0f;

	const float scale = 250.0f;
	lightPosition.x = x * scale - 278;
	lightPosition.y = y * scale + 278;

	//std::cout << lightPosition.x << " " << lightPosition.y << " " << lightPosition.z << std::endl;
}
