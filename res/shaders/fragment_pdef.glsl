#version 330
#pragma optimize(off)
#pragma debug(on)
#define MAX_SAMPLE_POINTS 128
layout(location = 1)out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;

uniform int aoMode;
uniform int noiseMode;
uniform vec3 hemisphere[MAX_SAMPLE_POINTS];
uniform mat4 projMat;

uniform vec2 noiseScale;
uniform sampler2D noiseTexture;
uniform int hemisphereSize;
uniform int hemisphereRadius;

vec4 toViewSpace( in vec2 uv ) {
    float z = texture(depth, uv).r;

    vec4 pos = inverse(projMat) * vec4(uv.x, uv.y, z, 1.0);
    return pos / pos.w;
}

void main()
{
    vec4 c = texture( colour, texcoord );
    vec3 norm = normalize(texture( normal, texcoord ).xyz);

	vec4 viewSpacePosition = toViewSpace(texcoord);
	vec4 randomSample = vec4(1,1,0,0);
	if (noiseMode == 1){
		randomSample = texture(noiseTexture, texcoord * noiseScale);
	}
    vec3 randomVec = normalize(randomSample.xyz);
	vec3 tangent = normalize(randomVec - norm * dot(randomVec, norm));
	mat3 transformationMatrix = mat3(tangent, cross(norm, tangent), norm);

	float occlusion = 0.0;
	for(int i = 0; i < hemisphereSize; i++) {
		vec4 samplePoint = vec4(transformationMatrix * hemisphere[i], 0.0);
		samplePoint = samplePoint * hemisphereRadius + viewSpacePosition;

		samplePoint = projMat * samplePoint;
		samplePoint /= samplePoint.w;
		vec2 sampleTexCoord = samplePoint.xy;

		float sampleDepth = texture(depth, sampleTexCoord).r;
		float delta = samplePoint.z - sampleDepth;

		float linearDepth = toViewSpace(sampleTexCoord).z;
		if (abs(viewSpacePosition.z - linearDepth) < hemisphereRadius && delta > 0.000005){
			occlusion++;
		}
	}

	occlusion = 1.0 - (occlusion / float(hemisphereSize));
	if (aoMode == 2){
		FBColor = c;
	}
	else if (aoMode == 1){
		FBColor = vec4(vec3(occlusion), 1.0);
	}
	else {
		FBColor = vec4(vec3(occlusion), 1.0) * c;
	}
}
