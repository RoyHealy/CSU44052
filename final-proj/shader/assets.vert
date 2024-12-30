#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;
layout(location = 5) in vec3 instanceOffsets;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
// out vec3 instanceNo;

uniform mat4 u_jointMat[25];
uniform mat4 MVP;

void main() {
    vec4 jointPos = vec4(vertexPosition+instanceOffsets, 1.0);
    worldNormal = vertexNormal; // 
    // Transform vertex
    //if (a_joint) {
    //    mat4 skinMat =
    //        a_weight.x * u_jointMat[int(a_joint.x)] +
    //        a_weight.y * u_jointMat[int(a_joint.y)] +
    //        a_weight.z * u_jointMat[int(a_joint.z)] +
    //        a_weight.w * u_jointMat[int(a_joint.w)];
    //    jointPos = skinMat * jointPos;
    //    worldNormal = normalize(skinMat * vec4(vertexNormal, 0)).xyz;
        // vec3 off = offsets[gl_InstanceID];
    //}
   
    worldPosition = jointPos.xyz;
    gl_Position =  MVP * jointPos;
    uv = vertexUV;
    
    // instanceNo = offsets[gl_InstanceID];
}
