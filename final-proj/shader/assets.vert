#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
out float instanceNo;

uniform vec3 offsets[25];
uniform mat4 MVP;

void main() {
    // Transform vertex
    vec4 jointPos = vec4(vertexPosition + offsets[gl_InstanceID], 1.0);
    worldPosition = jointPos.xyz;///jointPos.w;
    gl_Position =  MVP * jointPos;
    uv = vertexUV;
    worldNormal = vertexNormal; // normalize(skinMat * vec4(vertexNormal, 0)).xyz;
    instanceNo = gl_InstanceID;
}
