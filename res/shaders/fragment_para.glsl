// fragment shader for simple Phongs Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

layout(location = 1) out vec4 colour;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 spec;

smooth in vec2 vUV;
smooth in vec3 vNormal;
smooth in vec3 vView;
smooth in vec3 vPos;
smooth in vec3 vLightDir;

//layout(depth_greater) out float gl_FragDepth;

uniform mat4 mvM;
uniform mat3 light;
uniform float parallaxScale;// [0.005, 0.05, 0.1]
uniform float parallaxMinLayer;
uniform float parallaxMaxLayer;
uniform sampler2D image;
uniform sampler2D heightmap;

const float heightScale = 0.02; // slider[0.005, 0.05, 0.1]
const float ShadowOffset = 0.05;
const vec3 DiffuseColor = vec3(1.0);
const vec3 SpecularColor = vec3(0.3);
const float SpecularPower = 300.0; // [10, 300, 1000]
const float NormalDepth = 1.0; // [0.1, 1, 3]

// http://content.gpwiki.org/D3DBook:(Lighting)_Blinn-Phong
vec4 phong(vec3 normal, vec3 viewer, vec3 lit, vec2 texCoord, float shadow)
{
    vec3 texColor = texture2D(image, texCoord).xyz;

    // ambient
    vec3 ambient = vec3(0.2);

    // diffuse
    float lambertianTerm = max(dot(-lit, normal), 0.0);
    vec3 diffuse = texColor * lambertianTerm;

    // specular
    vec3 reflected = reflect(normal, -lit);
    float RdotV = max(dot(reflected, viewer), 0.0);
    vec3 specular = SpecularColor * pow(RdotV, SpecularPower);

    return vec4((ambient + diffuse * shadow + specular) * texColor, 1.0);
}

vec3 getNormal(vec2 p) {
    float s01 = textureOffset(heightmap, p, ivec2(-1, 0)).x;
    float s21 = textureOffset(heightmap, p, ivec2(1, 0)).x;
    float s10 = textureOffset(heightmap, p, ivec2(0, -1)).x;
    float s12 = textureOffset(heightmap, p, ivec2(0, 1)).x;

    // Central Difference Method from:
    // http://www.iquilezles.org/www/articles/terrainmarching/terrainmarching.htm
    vec3 n = vec3(s01 - s21, s10 - s12, NormalDepth);
    return normalize(n);
}

vec2 raymarch(vec2 startPos, vec3 dir) {
    // Compute initial parallax displacement direction:
    vec2 parallaxDirection = normalize(dir.xy);

    // The length of this vector determines the
    // furthest amount of displacement:
    float parallaxLength = sqrt( 1.0 - (dir.z * dir.z) );
    parallaxLength /= dir.z;

    // Compute the actual reverse parallax displacement vector:
    vec2 parallaxOffset = parallaxDirection * parallaxLength;

    // Need to scale the amount of displacement to account
    // for different height ranges in height maps.
    parallaxOffset *= parallaxScale;

    // corrected for tangent space. Normal is always z=1 in TS and
    // v.viewdir is in tangent space as well...
    int numSteps = int( mix( parallaxMaxLayer, parallaxMinLayer, dir.z ) );

    float currHeight = 0.0;
    float stepSize = 1.0 / float(numSteps);
    int stepIndex = 0;
    vec2 texCurrentOffset = startPos;
    vec2 texOffsetPerStep = stepSize * parallaxOffset;

    vec2 resultTexPos = vec2(texCurrentOffset - (texOffsetPerStep * numSteps));

    float prevHeight = 1.0;
    float currRayDist = 1.0;

    while (stepIndex < numSteps) {
        // Determine where along our ray we currently are.
        currRayDist -= stepSize;
        texCurrentOffset -= texOffsetPerStep;
        currHeight = texture( heightmap, texCurrentOffset ).r;

        // Because we're using heights in the [0..1] range
        // and the ray is defined in terms of [0..1] scanning
        // from top-bottom we can simply compare the surface
        // height against the current ray distance.
        if (currHeight >= currRayDist) {
            // Push the counter above the threshold so that
            // we exit the loop on the next iteration
            stepIndex = numSteps + 1;

            // We now know the location along the ray of the first
            // point *BELOW* the surface and the previous point
            // *ABOVE* the surface:
            float rayDistAbove = currRayDist + stepSize;
            float rayDistBelow = currRayDist;

            // We also know the height of the surface before and
            // after we intersected it:
            float surfHeightBefore = prevHeight;
            float surfHeightAfter = currHeight;

            float numerator = rayDistAbove - surfHeightBefore;
            float denominator = (surfHeightAfter - surfHeightBefore)
                    - (rayDistBelow - rayDistAbove);

            // As the angle between the view direction and the
            // surface becomes closer to parallel (e.g. grazing
            // view angles) the denominator will tend towards zero.
            // When computing the final ray length we'll
            // get a divide-by-zero and bad things happen.
            float x = 0.0;

            if (abs(denominator) > 1e-5) {
                x = numerator / denominator;
            }

            // Now that we've found the position along the ray
            // that indicates where the true intersection exists
            // we can translate this into a texture coordinate
            // - the intended output of this utility function.

            resultTexPos = mix(texCurrentOffset + texOffsetPerStep, texCurrentOffset, x);
        } else {
            ++stepIndex;
            prevHeight = currHeight;
        }
    }

    return resultTexPos;
}

float raymarchShadow(vec2 startPos, vec3 dir) {
    vec2 parallaxDirection = normalize(dir.xy);

    float parallaxLength = sqrt(1.0 - dir.z * dir.z);
    parallaxLength /= dir.z;

    vec2 parallaxOffset = parallaxDirection * parallaxLength;
    parallaxOffset *= parallaxScale;

    int numSteps = int(mix(parallaxMaxLayer, parallaxMinLayer, dir.z));

    float currHeight = 0.0;
    float stepSize = 1.0 / float(numSteps);
    int stepIndex = 0;

    vec2 texCurrentOffset = startPos;
    vec2 texOffsetPerStep = stepSize * parallaxOffset;

    float initialHeight = texture( heightmap, startPos ).r + ShadowOffset;

    while (stepIndex < numSteps) {
        texCurrentOffset += texOffsetPerStep;

        float rayHeight = mix(initialHeight, 1.0, stepIndex / numSteps);

        currHeight = texture(heightmap, texCurrentOffset).r;

        if (currHeight > rayHeight) {
            // ray has gone below the height of the surface, therefore
            // this pixel is occluded...
            return 0.0;
        }

        ++stepIndex;
    }

    return 1.0;
}

vec4 steepParallax(vec3 V, vec3 L, vec2 T) {
    vec3 result = vec3(1.0);
    float shadow = 1.0;
    T = raymarch(T, V);

    //if (UseShadow) {
        shadow = raymarchShadow(T, -L);
    //}
    vec3 N = getNormal(T);

    return phong(N, V, L, T, shadow);
}


vec4 offsetParallax(vec3 V, vec3 L, vec2 T) {
    // Compute the height at this location
    float height = texture(heightmap, T).x;
    height = heightScale * height - (heightScale * 0.5);

    // Compute the offset
    vec2 offsetDir = V.xy; // normalize( v.viewdir ).xy;
    T = T + offsetDir * height;

    // Take the samples with the shifted offset
    vec3 N = getNormal(T).xyz;

    return phong(N, V, L, T, 1.0);
}

vec4 bumpMapping(vec3 V, vec3 L, vec2 T) {
    vec3 N = getNormal(T);

    return phong(N, V, L, T, 1.0);
}


// https://www.opengl.org/discussion_boards/showthread.php/162857-Computing-the-tangent-space-in-the-fragment-shader
mat3 computeTangentSpaceMatrix() {
    vec3 A = dFdx(vPos);
    vec3 B = dFdy(vPos);

    vec2 P = dFdx(vUV);
    vec2 Q = dFdy(vUV);

    // Formula from:
    // http://content.gpwiki.org/D3DBook:(Lighting)_Per-Pixel_Lighting#Moving_From_Per-Vertex_To_Per-Pixel
    float fraction = 1.0f / (P.x * Q.y - Q.x * P.y);
    vec3 normal = normalize(cross(A, B));

    vec3 tangent = vec3(
            (Q.y * A.x - P.y * B.x) * fraction,
            (Q.y * A.y - P.y * B.y) * fraction,
            (Q.y * A.z - P.y * B.z) * fraction);

    vec3 bitangent = vec3(
            (P.x * B.x - Q.x * A.x) * fraction,
            (P.x * B.y - Q.x * A.y) * fraction,
            (P.x * B.z - Q.x * A.z) * fraction);

    // Some simple aliases
    float NdotT = dot(normal, tangent);
    float NdotB = dot(normal, bitangent);
    float TdotB = dot(tangent, bitangent);

    // Apply Gram-Schmidt orthogonalization
    tangent = tangent - NdotT * normal;
    bitangent = bitangent - NdotB * normal - TdotB * tangent;

    // Pack the vectors into the matrix output
    mat3 tsMatrix;

    tsMatrix[0] = tangent;
    tsMatrix[1] = bitangent;
    tsMatrix[2] = normal;

    return transpose(tsMatrix);
}

void main()
{
	mat3 tsMatrix = computeTangentSpaceMatrix();

    vec2 T = vUV;
    vec3 L = normalize(tsMatrix * vLightDir);
    vec3 V = normalize(tsMatrix * vView);
    float pos = length(vPos);

    if (pos > 500) 
    {
        vec3 N = normalize(tsMatrix * vNormal);
        colour = phong(N, V, L, T, 1.0);
    }
    else if (pos > 200 || parallaxScale == 0) colour = bumpMapping(V, L, T);
    //else colour = offsetParallax(V, L, T);
    else if (pos > 100) colour = offsetParallax(V, L, T);
    else colour = steepParallax(V, L, T);
}
