#version 330

#define MAX_KERNEL_SIZE 128
//layout(location = 1)
out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;

uniform vec2 pixelSize;
uniform int aoMode;
uniform vec3 kernel[MAX_KERNEL_SIZE];
uniform mat4 projMat;

uniform vec2 noiseScale;
uniform sampler2D noiseTexture;
uniform int kernelSize;
uniform int kernelRadius;

uniform float zoom;

vec4 getViewSpacePosition( in vec2 uv ) {
    float x = uv.s;
    float y = uv.t;
    float z = texture(depth, uv).r;

    vec4 pos = inverse(projMat) * vec4(x, y, z, 1.0);
    pos /= pos.w;

    return pos;
}

void main()
{
    float d = texture( depth, texcoord ).r;
    vec4 c = texture( colour, texcoord );
    vec4 n = texture( normal, texcoord );

    vec3 normal = normalize(n.xyz);

	// Caclulate view space position and normal
	vec4 viewSpacePosition = getViewSpacePosition(texcoord);
	vec4 randomSample = texture(noiseTexture, texcoord*noiseScale);
    vec3 randomVec = normalize(randomSample.xyz);
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 transformationMatrix = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; i++) {
		// Get sample position
		vec4 samplePoint = vec4(transformationMatrix * kernel[i], 0.0);
		samplePoint = samplePoint * kernelRadius + viewSpacePosition;
		float z = samplePoint.z;

		// Project sample position;
		samplePoint = projMat * samplePoint;
		samplePoint /= samplePoint.w;
		vec2 sampleTexCoord = samplePoint.xy;

		// Get sample depth
		float sampleDepth = texture(depth, sampleTexCoord).r;
		float delta = samplePoint.z - sampleDepth;

		// Get real depth and check if sample is within the kernel radius
		float linearDepth = getViewSpacePosition(sampleTexCoord).z;
		float rangeCheck = abs(viewSpacePosition.z - linearDepth) < kernelRadius ? 1.0 : 0.0;

		// Contribute to occlusion if within radius and larger than a small number to prevent crazy artifacts
		occlusion += (delta > 0.0000 ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / float(kernelSize));
	FBColor = vec4(vec3(occlusion), 1.0);
}