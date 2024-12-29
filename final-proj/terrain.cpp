#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb/stb_image.h>

#include <format>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <render/shader.h>

static float perlin(float x,float y);
static GLuint generatePerlinNoiseTexture(int layers, int chunkX, int chunkY);
static GLuint generatePerlinNoiseBuffer(int layers, int chunkX, int chunkY);

static const int gridSize = 6, width = 201, height = 201;
static const float chunkSize = 4000.f;
// static std::vector<std::vector<glm::vec2>> map;
// static 
static int indexBufferSize = (width-1)*(height-1)*6;
static glm::vec3 scaled = glm::vec3(chunkSize/(width-1),1,chunkSize/(height-1));
static glm::vec2 offset[] = {{-1,-1},{-1,0},{-1,1},
                             {0,-1}, {0,0}, {0,1},
                             {1,-1}, {1,0}, {1,1}};



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

struct Terrain {
    std::map<std::string, GLuint> perlinTextures = {};
    GLuint terrainProgramID;
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint heightMapID;
    GLuint MVPID;
    GLuint tmpID;

    void initialise() {
        GLfloat mesh[(width)*(height-1)*4]; // double up on y positions for more locality
        for (int y = 0; y < (height-1); y++) {
            for (int x = 0; x < width; x++) {
                int i = y*(width)*4+x*4;
                mesh[i] = x; mesh[i+1] = y; 
                mesh[i+2] = x; mesh[i+3] = y+1;
            }
        }
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);

        GLuint indexBuffer[indexBufferSize];
        for (int y = 0; y < height-1; y++) {
            for (int x = 0; x < width-1; x++) {
                int i = y*height*2+x*2; // location of vec2
                int ii = y*(height-1)*6 + x*6;
                indexBuffer[ii] = 3+i;    indexBuffer[ii+1] = 2+i;  indexBuffer[ii+2] = i; 
                indexBuffer[ii+3] = 3+i;  indexBuffer[ii+4] = i;    indexBuffer[ii+5] = i+1;
            }
        }

        // Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBuffer), indexBuffer, GL_STATIC_DRAW);

        terrainProgramID = LoadShadersFromFile("../final-proj/shader/terrain.vert", "../final-proj/shader/terrain.frag");
		if (terrainProgramID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}
        heightMapID = glGetUniformLocation(terrainProgramID, "sampleHeightMap");
        MVPID = glGetUniformLocation(terrainProgramID, "MVP");

        for (auto off : offset) {
            tmpID = getChunk(off.x-1, off.y-1);
        }
    }

    GLuint getChunk(int chunkX, int chunkY) {
        // std::string filename = "cache/" + std::to_string(chunkX) + "-" + std::to_string(chunkY) + ".bin";

        // std::ofstream fileee(filename.c_str(), std::ios::binary);
        std::string cachedName = std::to_string(chunkX) + "#" + std::to_string(chunkY);
        if (perlinTextures.find(cachedName) != perlinTextures.end()) {
            return perlinTextures[cachedName];
        }
        std::cout << "new chunk " + cachedName << std::endl;
        GLuint perlinId = generatePerlinNoiseTexture(gridSize, chunkX, chunkY);
        perlinTextures[cachedName] = perlinId;
        return perlinId;
    }

    void render(glm::vec3 cameraPosition, glm::mat4 viewpoint) {
        glm::vec3 chunkPosition = cameraPosition/chunkSize;
        int chunkX = (int)chunkPosition.x, chunkY = (int)chunkPosition.z;
        if (chunkPosition.x-chunkX < 0.5f) {
            chunkX -= 1;
        }
        if (chunkPosition.z-chunkY < 0.5f) {
            chunkY -= 1;
        }
        // std::cout << "pos (" << cameraPosition.x << ", " << cameraPosition.z << ")" << std::endl;
        for (glm::vec2 off : offset) {
            int x = chunkX+off.x;
            int y = chunkY+off.y;
            // std::cout << "chunk (" << x << ", " << y << ")" << std::endl;
            GLuint heightMap = getChunk(x, y);
            glUseProgram(terrainProgramID);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

            // glEnableVertexAttribArray(1);
            // glBindBuffer(GL_ARRAY_BUFFER, heightMapID);
            // glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

            glm::mat4 modelMatrix = glm::mat4();    
            // Scale the box along each axis to make it look like a building
            modelMatrix = glm::translate(modelMatrix, glm::vec3(x,0,y)*chunkSize);
            modelMatrix = glm::scale(modelMatrix, scaled); // lets hope the order is right
            // Set model-view-projection matrix
            glm::mat4 mvp = viewpoint * modelMatrix;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &mvp[0][0]);

            // skybox is using texture unit 0
            // Set textureSampler to use texture unit 1
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tmpID);
            glUniform1i(heightMapID, 0); 
            // ------------------------------------------

            // Draw the box
            glDrawElements(
                GL_TRIANGLES,      // mode
                indexBufferSize,    			   // number of indices
                GL_UNSIGNED_INT,   // type
                (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
        }
    }
};
// static float randf() {
//     float r = static_cast<float>(rand());
//     return r;//(r/RAND_MAX)*2.f - 1.f;// between -1 and 1
// }
// static unsigned seedx, seedy, seedz;
 // This function retrieves and stores the depth map of the default frame buffer 
// or a particular frame buffer (indicated by FBO ID) to a PNG image.

static std::vector<unsigned char> generatePerlinNoiseMap(int layers, int chunkX, int chunkY) {
    int channels = 3; 
    
    std::vector<float> depth(width * height*4);
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // glReadBuffer(GL_DEPTH_COMPONENT);
    // glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned char> img(width * height * 3);
	// glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());

    // seedx = random(), seedy = random(), seedz = random();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y*height+x;

            float val = 0;
            float freq = 3, amp = 1;
            for (int oct = 0; oct < layers; oct++) {
                val += perlin((float)x*freq/(width-1) + chunkX,(float)y*freq/(height-1) + chunkY) * amp; // -1 so it can connect chunks
                freq *= 2;
                amp  /= 2;
            }
            val *= 1.2;
            if (val > 1.f) val = 1.f;
            if (val < -1.f) val = -1.f;
            img[3*i] = img[3*i+1] = img[3*i+2] = (int) ((val+1.f)* 0.5f * 255);
        }
    }
    return img;
    // stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}

static GLuint generatePerlinNoiseBuffer(int layers, int chunkX, int chunkY) {
    std::vector<unsigned char> perlmap = generatePerlinNoiseMap(layers, chunkX, chunkY);
    // GLfloat buffer[img.size()/3];
    // for (int i = 0; i < img.size()/3; i++) buffer[i] = img[i*3];
    // u_int8_t tex = stbi_load_from_memory();
    int w, h, nc;
    uint8_t* img = stbi_load_from_memory(perlmap.data(), perlmap.size(), &w, &h, &nc, 3);
    GLuint heightBuffer;
    glGenBuffers(1, &heightBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, heightBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    
    return heightBuffer;
}

static GLuint generatePerlinNoiseTexture(int layers, int chunkX, int chunkY) {
    std::vector<unsigned char> img = generatePerlinNoiseMap(layers, chunkX, chunkY);
    int w, h, nc;
    uint8_t* img2 = stbi_load_from_memory(img.data(), img.size(), &w, &h, &nc, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

glm::vec2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443;//seedx;//
 
    b ^= a << s | a >> w - s;
    b *= 1911520717;//seedy;//
 
    a ^= b << s | b >> w - s;
    a *= 2048419325;//seedz;//
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    
    // Create the vector from the angle
    glm::vec2 v;
    v.x = sin(random);
    v.y = cos(random);
 
    return v;
}
// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    glm::vec2 gradient = randomGradient(ix, iy);
 
    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
 
    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}
 
float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
 
static float perlin(float x,float y) {
    // 4 grid locations
    int x0 = (int)x, y0 = (int)y;
    int x1 = x0+1,   y1 = y0+1;
    // weights for interpolating
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    return interpolate(ix0,ix1,sy);
}



// int main(void) {
    
//     // generatePerlinNoise("../perlin.png", gridSize, 0, 0);
//     // generatePerlinNoise("../perlin1.png", gridSize, 1, 0);
//     // generatePerlinNoise("../perlin01.png", gridSize, 0, 1);
//     // generatePerlinNoise("../perlin11.png", gridSize, 1,1);
//     GLFWwindow *window;
//     // Initialise GLFW
// 	if (!glfwInit())
// 	{
// 		std::cerr << "Failed to initialize GLFW." << std::endl;
// 		return -1;
// 	}

// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // Open a window and create its OpenGL context
// 	// window = glfwCreateWindow(windowWidth, windowHeight, "Lab 3", NULL, NULL);
// 	window = glfwCreateWindow(400, 400, "Lab 3", NULL, NULL);
// 	if (window == NULL)
// 	{
// 		std::cerr << "Failed to open a GLFW window." << std::endl;
// 		glfwTerminate();
// 		return -1;
// 	}

// 	glfwMakeContextCurrent(window);
    
    
//     // std::ofstream mesh("terrainVert.bin", std::ios::out|std::ios::binary);

    
//     return 0;
// }