#version 330
#define MAX_SAMPLE_POINTS 128
layout(location = 1) out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;

uniform int aoMode;
uniform vec3 hemisphere[MAX_SAMPLE_POINTS];
uniform mat4 projMat;

uniform vec2 noiseScale;
uniform sampler2D noiseTexture;
uniform int hemisphereSize;
uniform int hemisphereRadius;

vec4 toViewSpace( in vec2 uv ) {
    float x = uv.s;
    float y = uv.t;
    float z = texture(depth, uv).r;

    vec4 pos = inverse(projMat) * vec4(x, y, z, 1.0);
    pos /= pos.w;

    return pos;
}

void main()
{
    vec4 c = texture( colour, texcoord );
    vec3 norm = normalize(texture( normal, texcoord ).xyz);

	vec4 viewSpacePosition = toViewSpace(texcoord);
	vec4 randomSample = texture(noiseTexture, texcoord * (noiseScale * 3));
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
		float rangeCheck = abs(viewSpacePosition.z - linearDepth) < hemisphereRadius ? 1.0 : 0.0;

		occlusion += (delta > 0.0005 ? 1.0 : 0.0) * rangeCheck;
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