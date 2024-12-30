#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 5) in vec3 instanceOffsets;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
// out vec3 instanceNo;

// uniform vec3 offsets[25];
uniform mat4 MVP;

void main() {
    // Transform vertex
    // vec3 off = offsets[gl_InstanceID];
    vec4 jointPos = vec4(vertexPosition+instanceOffsets, 1.0);
    worldPosition = jointPos.xyz;///jointPos.w;
    gl_Position =  MVP * jointPos;
    uv = vertexUV;
    worldNormal = vertexNormal; // normalize(skinMat * vec4(vertexNormal, 0)).xyz;
    // instanceNo = offsets[gl_InstanceID];
}
