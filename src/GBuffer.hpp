#pragma once
#include <GL/glew.h>
#include <assert.h>
#include <vector>
#include <map>

#define GBUFFER_POSITION_TEXTURE_UNIT 0
#define GBUFFER_DIFFUSE_TEXTURE_UNIT  1
#define GBUFFER_NORMAL_TEXTURE_UNIT   2
#define GBUFFER_SPECULAR_TEXTURE_UNIT 3

namespace R308
{
class GBuffer
{
public:

    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_SPECULAR,
		GBUFFER_TEXTURE_TYPE_AUX0,
		GBUFFER_TEXTURE_TYPE_AUX1,
		GBUFFER_TEXTURE_TYPE_AUX2,
		GBUFFER_TEXTURE_TYPE_AUX3,
		GBUFFER_TEXTURE_MAX
    };
    
    struct FBObj
	{
    	GLuint fboID = 0, depthID = 0
    		 , xres = 0, yres = 0, numTex = 0
    		 , colorID[GBUFFER_TEXTURE_MAX] = { 0 };
	};

    GBuffer *getInstance();

    ~GBuffer();

    uint addBuffer( uint screenWidth, uint screenHeight, uint num );

    void BindForWriting( uint fb );
   
    void BindForReading( uint fb );
    
    void BindTextures( uint fb  );

private:
    static GBuffer *instance;
    GBuffer();
    std::map<GLuint, FBObj> framebuffer;
};

} // end R308 namespace

