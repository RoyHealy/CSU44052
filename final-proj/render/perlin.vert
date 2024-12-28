#version 330 core

// Input
layout(location = 0) in vec2 pos;

// Output data, to be interpolated for each fragment
out vec2 position;

void main() {
     gl_Position =  MVP * vec4(pos, 1.0, 1.0);4
     position = pos;
}
