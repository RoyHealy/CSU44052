#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
// #include <random>
#include <iostream>

static const int gridSize = 10;
// static std::vector<std::vector<glm::vec2>> map;
static GLFWwindow *window;

// static float randf() {
//     float r = static_cast<float>(rand());
//     return (r/RAND_MAX)*2.f - 1.f;// between -1 and 1
// }
 
glm::vec2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325;
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

// static void generatePerlinNoise(std::vector<std::vector<glm::vec2>> &map, int size) {
//     for (int i = 0; i < size; i++) {
//         std::vector<glm::vec2> row;
//         for (int j = 0; j < size; j++) {
//             row.push_back(glm::normalize(glm::vec2(randf(),randf())));
//         }
//         map.push_back(row);

//         // *map = glm::normalize(glm::vec2(randf(),randf()));
//     }
// }

int main(void) {
    // generatePerlinNoise(map, gridSize);
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
	window = glfwCreateWindow(400, 400, "Lab 3", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    return 0;
}