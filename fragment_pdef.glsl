#version 330

#define MAX_KERNEL_SIZE 128
out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;
uniform sampler2D eye;

uniform vec2 pixelSize;
uniform int aoMode;
uniform vec3 kernel[MAX_KERNEL_SIZE];
uniform mat4 projMat;

uniform vec2 noiseScale;
uniform sampler2D noiseTexture;
uniform int kernelSize;
uniform int kernelRadius;

uniform float zoom;

vec4 getViewSpacePosition(vec2 uv) {
    float x = uv.s * 2.0 - 1.0;
    float y = uv.t * 2.0 - 1.0;
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
    vec4 e = texture( eye, texcoord );
    FBColor = c;

//    vec3 normal = normalize(n.xyz * 2.0 - 1.0);
//
//	// Caclulate view space position and normal
//	vec4 viewSpacePosition = getViewSpacePosition(texcoord);
//    vec3 randomVec = normalize(texture(noiseTexture, texcoord * noiseScale).xyz);
//	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
//	vec3 bitangent = cross(normal, tangent);
//	mat3 transformationMatrix = mat3(tangent, bitangent, normal);
//
//	float occlusion = 1.0;
//	occlusion = 0.0;
//	for(int i = 0; i < kernelSize; i++) {
//		// Get sample position
//		vec4 samplePoint = vec4(transformationMatrix * kernel[i], 0.0);
//		samplePoint = samplePoint * kernelRadius + viewSpacePosition;
//		float z = samplePoint.z;
//
//		// Project sample position;
//		samplePoint = projMat * samplePoint;
//		samplePoint /= samplePoint.w;
//		vec2 sampleTexCoord = samplePoint.xy * 0.5 + 0.5;
//
//		// Get sample depth
//		float sampleDepth = texture(depth, sampleTexCoord).r;
//		float delta = samplePoint.z - sampleDepth;
//
//		// Get real depth and check if sample is within the kernel radius
//		float linearDepth = getViewSpacePosition(sampleTexCoord).z;
//		float rangeCheck = abs(viewSpacePosition.z - linearDepth) < kernelRadius ? 1.0 : 0.0;
//
//		// Contribute to occlusion if within radius and larger than a small number to prevent crazy artifacts
//		occlusion += (delta > 0.00005 ? 1.0 : 0.0) * rangeCheck;
//	}
//
//	occlusion = 1.0 - (occlusion / float(kernelSize));
//
//	FBColor = vec4(vec3(occlusion), 1.0);


	float d2 = d - 0.01;
	if (d == 1){
		FBColor = vec4(0,0,0,1);
		return;
	}

	float threshold = 1.0;
	float range = 50.0;
	float strength = 0.005;
	float multiplier = threshold;
	float end = (range*(1/zoom));

	for (float i=0; i<=end; i+=1/zoom){
		float diagonalStep = sin(0.785398)*i;
		float left  = texture(depth, vec2(texcoord.x-pixelSize.x * i, texcoord.y)).r;
		float right = texture(depth, vec2(texcoord.x+pixelSize.x * i, texcoord.y)).r;
		float up    = texture(depth, vec2(texcoord.x, texcoord.y+pixelSize.y * i)).r;
		float down  = texture(depth, vec2(texcoord.x, texcoord.y-pixelSize.y * i)).r;

		float upLeft    = texture(depth, vec2(texcoord.x-pixelSize.x * diagonalStep, texcoord.y+pixelSize.y * diagonalStep)).r;
		float upRight   = texture(depth, vec2(texcoord.x+pixelSize.x * diagonalStep, texcoord.y+pixelSize.y * diagonalStep)).r;
		float downLeft  = texture(depth, vec2(texcoord.x-pixelSize.x * diagonalStep, texcoord.y-pixelSize.y * diagonalStep)).r;
		float downRight = texture(depth, vec2(texcoord.x+pixelSize.x * diagonalStep, texcoord.y-pixelSize.y * diagonalStep)).r;

		if (d > up && d2 < up && d > down && d2 < down){
			multiplier -= strength;
		}
		if (d > left && d2 < left && d > right && d2 < right){
			multiplier -= strength;
		}
		if (d > upLeft && d2 < upLeft && d > downRight && d2 < downRight){
			multiplier -= strength;
		}
		if (d > downLeft && d2 < downLeft && d > upRight && d2 < upRight){
			multiplier -= strength;
		}
	}


	if (aoMode == 1 && texcoord.x > 0.5){
		FBColor = max(multiplier, 0.2) * vec4(1,1,1,1);
	}
	else if (aoMode == 2){
		FBColor = max(multiplier, 0.2) * vec4(1,1,1,1);
	}
	else {
		FBColor = max(multiplier, 0.2) * c;
	}
}