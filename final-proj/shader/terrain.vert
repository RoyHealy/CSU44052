#version 330 core

// Input
layout(location = 0) in vec2 vertexPosition;
// layout(location = 1) in float heightMapped;

// Output data, to be interpolated for each fragment
out float height;


uniform sampler2D sampleHeightMap;
uniform mat4 MVP;

float chunkSize = 201.f;

void main() {
    vec2 uv = (vertexPosition/chunkSize);
    height = texture(sampleHeightMap, uv).r;
    // height = heightMapped;
    vec4 jointPos = vec4(vertexPosition.x, height*100, vertexPosition.y, 1.0);

    gl_Position =  MVP * jointPos;
}
