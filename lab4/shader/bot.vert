#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;

uniform mat4 MVP;
uniform mat4 u_jointMat[25];

// attribute vec4 a_position;
// attribute vec4 a_joint; // probably not vec4
// attribute vec4 a_weight;

void main() {
    // Transform vertex
    mat4 skinMat =
        a_weight.x * u_jointMat[int(a_joint.x)] +
        a_weight.y * u_jointMat[int(a_joint.y)] +
        a_weight.z * u_jointMat[int(a_joint.z)] +
        a_weight.w * u_jointMat[int(a_joint.w)];
    vec4 jointPos = skinMat * vec4(vertexPosition, 1.0);
    worldPosition = jointPos.xyz;///jointPos.w;
     gl_Position =  MVP * jointPos;
   //  gl_Position = MVP * vec4(vertexPosition, 1.0);
    // gl_Position = MVP * skinMat * vec4(vertexPosition, 1.0);
    // gl_Position = MVP * u_jointMat[int(a_joint.x)][3];
    // World-space geometry 
    // worldPosition = vertexPosition;
    // worldNormal = vertexNormal;
    worldNormal = normalize(skinMat * vec4(vertexNormal, 0)).xyz;
}