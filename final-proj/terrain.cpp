#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>

#include <format>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <render/shader.h>

static float perlin(float x,float y);
static int generatePerlinNoiseTexture(int layers, int chunkX, int chunkY);

static const int gridSize = 6, width = 200, height = 200;
static const float chunkSize = 2000.f;
// static std::vector<std::vector<glm::vec2>> map;
// static 
static int indexBufferSize = (width-1)*(height-1)*6;

static glm::vec2 offset[] = {glm::vec2(0,0),glm::vec2(1,0),glm::vec2(0,1),glm::vec2(1,1)};

struct Terrain {
    std::map<std::string, GLuint> perlinTextures = {};
    GLuint terrainProgramID;
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint heightMapID;
    GLuint MVPID;
    GLuint chunkID;

    void initialise() {
        GLfloat mesh[(width)*(height-1)*4]; // double up on y positions for more locality
        for (int y = 0; y < (height-1); y++) {
            for (int x = 0; x < width; x++) {
                int i = y*height*4+x*4;
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
        chunkID = glGetUniformLocation(terrainProgramID, "chunk");
        MVPID = glGetUniformLocation(terrainProgramID, "MVP");
        for (auto off : offset) {
            getChunk(off.x-1, off.y-1);
        }
    }

    GLuint getChunk(int chunkX, int chunkY) {
        // std::string filename = "cache/" + std::to_string(chunkX) + "-" + std::to_string(chunkY) + ".bin";

        // std::ofstream(filename.c_str(), std::ios::binary);
        std::string cachedName = std::to_string(chunkX) + "#" + std::to_string(chunkY);
        if (perlinTextures.find(cachedName) != perlinTextures.end()) {
            return perlinTextures[cachedName];
        }
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
        for (auto off : offset) {
            int x = chunkX+off.x;
            int y = chunkX+off.x;
            std::cout << "chunk (" << x << ", " << y << ")" << std::endl;
            GLuint heightMap = getChunk(x, y);
            glUseProgram(terrainProgramID);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

            glm::mat4 modelMatrix = glm::mat4();    
            // Scale the box along each axis to make it look like a building
            modelMatrix = glm::translate(modelMatrix, glm::vec3(x,0,y)*chunkSize);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(chunkSize/width,1,chunkSize/height)); // lets hope the order is right
            // Set model-view-projection matrix
            glm::mat4 mvp = viewpoint * modelMatrix;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &mvp[0][0]);

            // Set textureSampler to use texture unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, heightMap);
            glUniform1i(heightMapID, 0); 
            // ------------------------------------------
            glUniform2f(chunkID, x, y);

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
                val += perlin((float)x*freq/width + chunkX,(float)y*freq/height + chunkY) * amp;
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

static int generatePerlinNoiseTexture(int layers, int chunkX, int chunkY) {
    std::vector<unsigned char> img = generatePerlinNoiseMap(layers, chunkX, chunkY);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data());
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