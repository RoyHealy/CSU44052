#version 330 core

in vec3 color;

// TODO: To add UV input to this fragment shader 
in vec2 uv;

// TODO: To add the texture sampler
uniform sampler2D textureSampler;

out vec3 finalColor;

void main()
{
	// finalColor = color;

	// TODO: texture lookup. 
	// TODO: Perform texture lookup.
	// Hint: use the GLSL texture() function
	// It takes as input the texture sampler 
	// and the UV coordinate and returns an RGB color.
	finalColor = texture(textureSampler, uv).rgb; 
	// For the existing vertex color variable, you can remove it, or
	// multiply its value with the texture, e.g.,
	// finalColor = color * texture(textureSampler, uv).rgb; 
}
