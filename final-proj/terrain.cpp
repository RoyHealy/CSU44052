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
// #include "buildings.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static float perlin(float x,float y);
static GLuint generatePerlinNoiseTexture(int layers, int chunkX, int chunkY);
static GLuint generatePerlinNoiseBuffer(int layers, int chunkX, int chunkY);

static const int gridSize = 6, width = 201, height = 201;
static const float chunkSize = 4000.f;
// static std::vector<std::vector<glm::vec2>> map;
// static 
static int indexBufferSize = (width-1)*(height-1)*6;
static glm::vec3 scaled = glm::vec3(chunkSize/(width-1),1,chunkSize/(height-1));
static glm::vec2 offset[] = //{{0,0}, {0,1}, {1,0}, {1,1}};
							{{-1,-1},{-1,0},{-1,1},
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
        // std::cout << "new chunk " + cachedName << std::endl;
        GLuint perlinId = generatePerlinNoiseTexture(gridSize, chunkX, chunkY);
        perlinTextures[cachedName] = perlinId;
        return perlinId;
    }

    void render(glm::vec3 cameraPosition, glm::mat4 viewpoint) {
        glm::vec3 chunkPosition = cameraPosition/chunkSize;
        int chunkX = (int)chunkPosition.x, chunkY = (int)chunkPosition.z;
        // if (chunkPosition.x-chunkX < 0.5f) {
        //     chunkX -= 1;
        // }
        // if (chunkPosition.z-chunkY < 0.5f) {
        //     chunkY -= 1;
        // }
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

// static GLuint generatePerlinNoiseBuffer(int layers, int chunkX, int chunkY) {
//     std::vector<unsigned char> perlmap = generatePerlinNoiseMap(layers, chunkX, chunkY);
//     // GLfloat buffer[img.size()/3];
//     // for (int i = 0; i < img.size()/3; i++) buffer[i] = img[i*3];
//     // u_int8_t tex = stbi_load_from_memory();
//     int w, h, nc;
//     uint8_t* img = stbi_load_from_memory(perlmap.data(), perlmap.size(), &w, &h, &nc, 3);
//     GLuint heightBuffer;
//     glGenBuffers(1, &heightBuffer);
//     glBindBuffer(GL_ARRAY_BUFFER, heightBuffer);
//     // glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    
//     return heightBuffer;
// }

static GLuint generatePerlinNoiseTexture(int layers, int chunkX, int chunkY) {
    std::vector<unsigned char> img = generatePerlinNoiseMap(layers, chunkX, chunkY);
    std::string filename = "tmp.png";
    stbi_write_png(filename.c_str(), width, height, 3, img.data(), width * 3);
    int w, h, nc;
    uint8_t* img2 = stbi_load(filename.c_str(), &w, &h, &nc, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (img2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load perlin texture " << std::endl;
    }
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




// Lighting  
static glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
static glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

// Animation 
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

struct MyAsset {
// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint jointMatricesID;
	GLuint lightPositionID;
	GLuint lightIntensityID;
	GLuint baseColorFactorID;
	GLuint textureID;
	GLuint programID;

	tinygltf::Model model;

	// Each VAO corresponds to each mesh primitive in the GLTF model
	struct PrimitiveObject {
		GLuint vao;
		std::map<int, GLuint> vbos;
	};
	std::vector<PrimitiveObject> primitiveObjects;

	// Skinning 
	struct SkinObject {
		// Transforms the geometry into the space of the respective joint
		std::vector<glm::mat4> inverseBindMatrices;  

		// Transforms the geometry following the movement of the joints
		std::vector<glm::mat4> globalJointTransforms;

		// Combined transforms
		std::vector<glm::mat4> jointMatrices;
	};
	std::vector<SkinObject> skinObjects;

	// Animation 
	struct SamplerObject {
		std::vector<float> input;
		std::vector<glm::vec4> output;
		int interpolation;
	};
	struct ChannelObject {
		int sampler;
		std::string targetPath;
		int targetNode;
	}; 
	struct AnimationObject {
		std::vector<SamplerObject> samplers;	// Animation data
	};
	std::vector<AnimationObject> animationObjects;

	glm::mat4 getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f); 

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
			if (node.translation.size() == 3) {
				transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
			}
			if (node.rotation.size() == 4) {
				glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				transform *= glm::mat4_cast(q);
			}
			if (node.scale.size() == 3) {
				transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
			}
		}
		return transform;
	}

	void computeLocalNodeTransform(const tinygltf::Model& model, 
		int nodeIndex, 
		std::vector<glm::mat4> &localTransforms)
	{
		// ---------------------------------------
		// TODO: your code here
		tinygltf::Node localNode = model.nodes[nodeIndex];
		localTransforms[nodeIndex] = getNodeTransform(localNode);
		// printf("local %d done\n", nodeIndex);
		for (int childIndex : localNode.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}
		// ---------------------------------------
	}

	void computeGlobalNodeTransform(const tinygltf::Model& model, 
		const std::vector<glm::mat4> &localTransforms,
		int nodeIndex, const glm::mat4& parentTransform, 
		std::vector<glm::mat4> &globalTransforms)
	{
		// ----------------------------------------
		// TODO: your code here
		tinygltf::Node localNode = model.nodes[nodeIndex];
		glm::mat4 globalTrans = parentTransform * localTransforms[nodeIndex];
		globalTransforms[nodeIndex] = globalTrans;
		// printf("global %d done\n", nodeIndex);
		for (int childIndex : localNode.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, globalTrans, globalTransforms);
		}
		// ----------------------------------------
	}

	std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model) {
		std::vector<SkinObject> skinObjects;

		// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject skinObject;

			const tinygltf::Skin &skin = model.skins[i];

			// Read inverseBindMatrices
			const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
			assert(accessor.type == TINYGLTF_TYPE_MAT4);
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			const float *ptr = reinterpret_cast<const float *>(
            	buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);
			
			skinObject.inverseBindMatrices.resize(accessor.count);
			for (size_t j = 0; j < accessor.count; j++) {
				float m[16];
				memcpy(m, ptr + j * 16, 16 * sizeof(float));
				skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
			}

			assert(skin.joints.size() == accessor.count);

			skinObject.globalJointTransforms.resize(skin.joints.size());
			skinObject.jointMatrices.resize(skin.joints.size());

			// ----------------------------------------------
			// TODO: your code here to compute joint matrices
			// ----------------------------------------------

			// Compute local transforms at each node
			int rootNodeIndex = skin.joints[0];
			std::vector<glm::mat4> localNodeTransforms(skin.joints.size());
			computeLocalNodeTransform(model, rootNodeIndex, localNodeTransforms);

			// Compute global transforms at each node
			glm::mat4 parentTransform(1.0f);
			// std::vector<glm::mat4> globalNodeTransforms(skin.joints.size());
			computeGlobalNodeTransform(model, localNodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);
			for (int j = 0; j < skin.joints.size(); j++) {
				int globalJointIndex = skin.joints[j];
				skinObject.jointMatrices[j] = skinObject.globalJointTransforms[globalJointIndex] * skinObject.inverseBindMatrices[j];
			}
			// ----------------------------------------------

			skinObjects.push_back(skinObject);
		}
		return skinObjects;
	}

	int findKeyframeIndex(const std::vector<float>& times, float animationTime) 
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model) 
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;
			
			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}
				
				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);
				
				// Output values
				samplerObject.output.resize(outputAccessor.count);
				
				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);			
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void updateAnimation(
		const tinygltf::Model &model, 
		const tinygltf::Animation &anim, 
		const AnimationObject &animationObject, 
		float time,
		std::vector<glm::mat4> &nodeTransforms) 
	{
		// There are many channels so we have to accumulate the transforms 
		for (const auto &channel : anim.channels) {
			
			int targetNodeIndex = channel.target_node;
			const auto &sampler = anim.samplers[channel.sampler];
			
			// Access output (value) data for the channel
			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			// Calculate current animation time (wrap if necessary)
			const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
			float animationTime = fmod(time, times.back());
			
			// ----------------------------------------------------------
			// TODO: Find a keyframe for getting animation data 
			// ----------------------------------------------------------
			int keyframeIndex = findKeyframeIndex(times, animationTime); 

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			// -----------------------------------------------------------
			// TODO: Add interpolation for smooth interpolation
			// -----------------------------------------------------------
			
			float interdistance = 0; // linear interpolation
			if (keyframeIndex>=0 && keyframeIndex < times.size()-2) {
				interdistance = (animationTime - times[keyframeIndex])/(times[keyframeIndex+1]-times[keyframeIndex]);
			}
			if (channel.target_path == "translation") {
				glm::vec3 translation0, translation1;
				memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&translation1, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));
				glm::vec3 translation = translation0 + interdistance*(translation1-translation0); // if using quaternions, glm::lerp(translation0, translation1, interdistance)
				nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
			} else if (channel.target_path == "rotation") {
				glm::quat rotation0, rotation1;
				memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));
				memcpy(&rotation1, outputPtr + (keyframeIndex+1) * 4 * sizeof(float), 4 * sizeof(float));
				glm::quat rotation = glm::slerp(rotation0, rotation1, interdistance);//rotation0;  + interdistance*(rotation1-rotation0);
				nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
			} else if (channel.target_path == "scale") {
				glm::vec3 scale0, scale1;
				memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&scale0, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));
				glm::vec3 scale = scale0 + interdistance*(scale1-scale0);
				nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
			}
		}
	}

	void updateSkinning(const std::vector<glm::mat4> &nodeTransforms) {

		// -------------------------------------------------
		// TODO: Recompute joint matrices 
		// -------------------------------------------------
		for (int i = 0; i < model.skins.size(); i++) {
			SkinObject &skinObject = skinObjects[i];
			const tinygltf::Skin &skin = model.skins[i];
			// Compute global transforms at each node
			glm::mat4 parentTransform(1.0f);
			int rootNodeIndex = skin.joints[0];
			computeGlobalNodeTransform(model, nodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);
			
			for (int j = 0; j < skin.joints.size(); j++) {
				int globalJointIndex = skin.joints[j];
				skinObject.jointMatrices[j] = skinObject.globalJointTransforms[globalJointIndex] * skinObject.inverseBindMatrices[j];
			}
		}
	}

	void update(float time) {

		// -------------------------------------------------
		// TODO: your code here 
		// -------------------------------------------------
		
		// maybe need for each skin
		// for (int i = 0; i < model.skins.size(); i++) {// supposedly theres only 1 skin, if there is multiple we should only render 1 of them
		if (model.animations.size() > 0) {
			const tinygltf::Animation &animation = model.animations[0];
			const AnimationObject &animationObject = animationObjects[0];

			const tinygltf::Skin &skin = model.skins[0];// was model.skin[0]
			std::vector<glm::mat4> nodeTransforms(skin.joints.size());
			for (size_t i = 0; i < nodeTransforms.size(); ++i) {
				nodeTransforms[i] = glm::mat4(1.0);
			}

			updateAnimation(model, animation, animationObject, time, nodeTransforms);
			
			updateSkinning(nodeTransforms);
		}
		// }
	}

	bool loadModel(tinygltf::Model &model, const char *filename) {
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cout << "ERR: " << err << std::endl;
		}

		if (!res)
			std::cout << "Failed to load glTF: " << filename << std::endl;
		else
			std::cout << "Loaded glTF: " << filename << std::endl;

		return res;
	}

	void initialize(const char* filename) {
		// Modify your path if needed
		if (!loadModel(model, filename)) {
			return;
		}
		// for (auto& buf : model.buffers) {
			
		// 	std::cout << buf.data.size() << std::endl;
		// }

		// Prepare buffers for rendering 
		primitiveObjects = bindModel(model);

		// Prepare joint matrices
		skinObjects = prepareSkinning(model);

		// Prepare animation data 
		animationObjects = prepareAnimation(model);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../final-proj/shader/assets.vert", "../final-proj/shader/assets.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}
		// Get a handle for GLSL variables
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		// jointMatricesID = glGetUniformLocation(programID, "u_jointMat");
		// textureID = glGetUniformLocation(programID, "textureSampler");
		baseColorFactorID = glGetUniformLocation(programID, "baseColorFactor");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
	}

	void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {

		std::map<int, GLuint> vbos;
		for (size_t i = 0; i < model.bufferViews.size(); ++i) {
			const tinygltf::BufferView &bufferView = model.bufferViews[i];

			int target = bufferView.target;
			
			if (bufferView.target == 0) { 
				// The bufferView with target == 0 in our model refers to 
				// the skinning weights, for 25 joints, each 4x4 matrix (16 floats), totaling to 400 floats or 1600 bytes. 
				// So it is considered safe to skip the warning.
				//std::cout << "WARN: bufferView.target is zero" << std::endl;
				continue;
			}

			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, bufferView.byteLength,
						&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
			
			vbos[i] = vbo;
		}

		// Each mesh can contain several primitives (or parts), each we need to 
		// bind to an OpenGL vertex array object
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			for (auto &attrib : primitive.attributes) {
				tinygltf::Accessor accessor = model.accessors[attrib.second];
				int byteStride =
					accessor.ByteStride(model.bufferViews[accessor.bufferView]);
				glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

				int size = 1;
				if (accessor.type != TINYGLTF_TYPE_SCALAR) {
					size = accessor.type;
				}

				int vaa = -1;
				if (attrib.first.compare("POSITION") == 0) vaa = 0;
				if (attrib.first.compare("NORMAL") == 0) vaa = 1;
				if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
				if (attrib.first.compare("JOINTS_0") == 0) vaa = 3;
				if (attrib.first.compare("WEIGHTS_0") == 0) vaa = 4;
				if (vaa > -1) {
					glEnableVertexAttribArray(vaa);
					glVertexAttribPointer(vaa, size, accessor.componentType,
										accessor.normalized ? GL_TRUE : GL_FALSE,
										byteStride, BUFFER_OFFSET(accessor.byteOffset));
				} else {
					std::cout << "vaa missing: " << attrib.first << std::endl;
				}
			}

			// Record VAO for later use
			PrimitiveObject primitiveObject;
			primitiveObject.vao = vao;
			primitiveObject.vbos = vbos;
			primitiveObjects.push_back(primitiveObject);

			glBindVertexArray(0);
		}
	}

	void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, 
						tinygltf::Model &model,
						tinygltf::Node &node) {
		// Bind buffers for the current mesh at the node
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}

		// Recursive into children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}

	std::vector<PrimitiveObject> bindModel(tinygltf::Model &model) {
		std::vector<PrimitiveObject> primitiveObjects;

		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}

		return primitiveObjects;
	}

	void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {
		
		for (size_t i = 0; i < mesh.primitives.size(); ++i) 
		{
			GLuint vao = primitiveObjects[i].vao;
			std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

			glBindVertexArray(vao);

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			std::vector<double> color = model.materials[primitive.material].pbrMetallicRoughness.baseColorFactor;
			std::vector<float> k;
			for (double d : color) k.push_back(d); 
			// std::cout << color[0] << " " << k[0] << std::endl;
			glUniform4fv(baseColorFactorID, 1, &k[0]);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
			glDrawElements(primitive.mode, indexAccessor.count,
						indexAccessor.componentType,
						BUFFER_OFFSET(indexAccessor.byteOffset));
			glBindVertexArray(0);
		}
	}

	void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
						tinygltf::Model &model, tinygltf::Node &node) {
		// Draw the mesh at the node, and recursively do so for children nodes
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}
		for (size_t i = 0; i < node.children.size(); i++) {
			drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}
	void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
				tinygltf::Model &model) {
		// Draw all nodes
		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}
	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);
		// Set camera
		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		// -----------------------------------------------------------------
		// TODO: Set animation data for linear blend skinning in shader
		// -----------------------------------------------------------------
		// glUniformMatrix4fv(jointMatricesID, 25, GL_FALSE, &skinObjects[0].jointMatrices[0][0][0]);
		// -----------------------------------------------------------------
		// Set light data 
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
		// Draw the GLTF model
		drawModel(primitiveObjects, model);
		const tinygltf::Skin &skin = model.skins[0];
	}

	void cleanup() {
		glDeleteProgram(programID);
	}
}; 