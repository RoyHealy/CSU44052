#version 330 core
in vec3 worldPos;
in float height;

out vec3 finalColor;

// uniform sampler2D grassTexture;
uniform sampler2D lightSource;
// uniform lightSource;
uniform mat4 lightMVP;

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
	if (height < 50) {
		finalColor = vec3(0.05f, 0.8f, 0.08f);
	}
	finalColor = height*vec3(0.3f, 0.95f, 0.4f);

	vec4 shadowPosition = lightMVP * vec4(worldPos, 1);
	vec3 lightSpaceXYZ = (shadowPosition.xyz/shadowPosition.w )*0.5 + 0.5; // ndc to uv space (for sampler2D)
	float existingDepth = texture(lightSource, lightSpaceXYZ.xy).r;

	float shadow = (lightSpaceXYZ.z >= existingDepth + 1e-2) ? 0.4 : 1.0;
	if (lightSpaceXYZ.z > 1.f) shadow = 1.f;
	finalColor *= shadow;
	// finalColor = vec3(existingDepth);
}
