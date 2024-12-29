#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;

// Output data, to be interpolated for each fragment
out vec3 position;

uniform mat4 MVP;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);
    
    // Pass vertex color to the fragment shader

    // World-space geometry 
    position = gl_Position.xyz;
}
