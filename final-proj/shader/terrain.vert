#version 330 core

// Input
layout(location = 0) in vec2 vertexPosition;
// layout(location = 1) in vec3 vertexNormal;
// layout(location = 2) in vec2 vertexUV;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
//out vec3 worldNormal;
//out vec2 uv;

uniform sampler2D sampleHeightMap;
uniform vec2 chunk;
uniform mat4 MVP;

float chunkSize = 2000.f;

void main() {
    vec2 uv = (vertexPosition/chunkSize)-chunk;
    float height = texture(sampleHeightMap, uv).r;
    // Transform vertex
    vec4 jointPos = vec4(vertexPosition.x, height, vertexPosition.y, 1.0);
    worldPosition = jointPos.xyz;///jointPos.w;
    gl_Position =  MVP * jointPos;
    // uv = vertexUV;
    // worldNormal = vertexNormal;
}
