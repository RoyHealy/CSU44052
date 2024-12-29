#version 330 core

out float finalColor;

in vec3 position;

uniform float farPlane;
// uniform sampler2D shadowMap;
// uniform mat4 lightSpaceMatrix; // might need to be taken in from vert

void main()
{
	// finalColor = color;
	
	// vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPosition, 1);

	// float existingDepth = texture(shadowMap, lightSpacePos.xy).r;

    // scale between zfar and znear
	// float shadow = position.z; // = 0?
	// shadowMap[position.xy] =  shadow/far_plane;
	// finalColor = shadow/farPlane;
	// gl_FragDepth = 255*finalColor;
}
