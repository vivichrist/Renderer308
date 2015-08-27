// fragment shader for simple Phongs Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

// uniform sampler2D Image;
uniform vec3 LightPosition; // Light position, in camera coordinates
uniform vec3 LightColor; // The color of our light

smooth in vec3 PositionInterp;
smooth in vec3 NormalInterp;
smooth in vec3 ColorInterp;
// smooth in vec2 TextCoordInterp;

out vec4 FBColor;

void main()
{
    /* After interpolation, normals probably are denormalized,
    so we need renormalize them */
    vec3 normal = normalize(NormalInterp);
    /*Calculate the rest of parameters exactly like we did in the
    vertex shader version of this shader*/
    vec3 lightVec = normalize(LightPosition - PositionInterp);
    vec3 viewVec = normalize(-PositionInterp);
    vec3 reflectVec = reflect(-lightVec, normal);
    
    float spec = max(dot(reflectVec, viewVec), 0.0);
    spec = pow(spec, 16.0);
    
    // vec4 textureColor = texture(Image, TextCoordInterp);
    vec3 specContrib = LightColor * spec;
    
    // No ambient contribution this time
    vec3 ambientContrib = vec3(0.0, 0.0, 0.0);
    
    vec3 diffContrib = ((ColorInterp + LightColor) * 0.5f) * max(dot(lightVec, normal), 0.0);
    /* Apply the mask to the specular contribution. The "1.0 –" is
    To invert the texture's alpha channel */
    vec3 lightContribution = ambientContrib + diffContrib + specContrib; 
    //(specContrib *(1.0 - textureColor.a));
    
    // FBColor = vec4(textureColor.rgb * lightContribution, 1.0);
    FBColor = vec4( ColorInterp * lightContribution, 1.0);
}