#version 330 core

in vec3 color;
in vec3 worldPosition;
in vec3 worldNormal; 

out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;

const float PI = 3.14159265358979323846;
const float TOTAL_ENERGY = 40.f; // 2-100 watts, this is how bright it is
const float BRDF = (0.6)/PI; // 0.0-1.0, intuitively this is how much it reflects
void main()
{
	// finalColor = color;
	vec4 shadowPosition = lightSpaceMatrix * vec4(worldPosition, 1);
	// TODO: lighting, tone mapping, gamma correction
	vec3 lightDirection = lightPosition-worldPosition;
	float r = length(lightDirection);

	float lightSourceIrradiance = TOTAL_ENERGY/(4.f * PI * r*r);
	
	// the cosine rule
	float cosTheta = dot(lightDirection, worldNormal) / (r); // length of worldNormal = 1
	
	// idk what BRDF should be (incidence power / reflected power)/pi
	vec3 reflectedRadiance = (BRDF * cosTheta * lightSourceIrradiance) * lightIntensity;

	// tone mapping of radiance
	// should this be done on the color*reflectedRadiance
	vec3 toneMapping = reflectedRadiance / (1 + reflectedRadiance);

	// gamma correction on tone mapping?
	vec3 gammaCorrection = pow(toneMapping, vec3(1/2.2, 1/2.2, 1/2.2)); 
	// 1/2.2 because 2.2 seems to make it darker (probably because it should a value between 0, and 1)
	// vec3(2.2, 2.2, 2.2) ; // vec3(1/2.2, 1/2.2, 1/2.2)
	
	vec3 lightSpaceXYZ = (shadowPosition.xyz/shadowPosition.w )*0.5 + 0.5; // ndc to uv space (for sampler2D)
	float existingDepth = texture(shadowMap, lightSpaceXYZ.xy).r;

	float shadow = (lightSpaceXYZ.z >= existingDepth + 1e-3) ? 0.2 : 1.0;
	if (lightSpaceXYZ.z > 1.f) shadow = 1.f;

	//finalColor = gammaCorrection*color*shadow;
	if (cosTheta <= 0.f) {
		finalColor = vec3(0.f,0.f,0.f);
	}
	else {
		finalColor = gammaCorrection*color*shadow;
	}
	
	// finalColor = color*existingDepth;
}
