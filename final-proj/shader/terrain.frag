#version 330 core

in vec3 worldPosition;
// in vec3 worldNormal; 
// in vec2 uv;

out vec3 finalColor;
// uniform sampler2D textureSampler;

void main()
{
	// Lighting
	//vec3 lightDir = lightPosition - worldPosition;
	//float lightDist = dot(lightDir, lightDir);
	//lightDir = normalize(lightDir);
	//vec3 v = lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;

	// Tone mapping
	//v = v / (1.0 + v);

	// Gamma correction
	//finalColor = vec4(pow(v, vec3(1.0 / 2.2)),1.0) * baseColorFactor;//texture(textureSampler,uv).rgb;
	finalColor = vec3(worldPosition.y);
}
