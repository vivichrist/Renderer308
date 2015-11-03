#include <stdio.h>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>   // file I/O
#include <exception>

#include "GBuffer.hpp"

using namespace std;
using namespace R308;

GBuffer *GBuffer::instance = nullptr;

GBuffer::GBuffer()
{
	framebuffer.clear();
}

GBuffer *GBuffer::getInstance()
{
	if ( instance == nullptr )
		instance = new GBuffer();
	return instance;
}

GBuffer::~GBuffer()
{
    for ( auto &fb: framebuffer ) {
        glDeleteFramebuffers(1, &fb.second.fboID);
        glDeleteTextures( fb.second.numTex, fb.second.colorID );
		glDeleteTextures( 1, &fb.second.depthID );
	}
}


uint GBuffer::addBuffer( uint screenWidth, uint screenHeight, uint num )
{
	assert( num < GBUFFER_TEXTURE_MAX );
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	FBObj fbo;
	glGenFramebuffers( 1, &fbo.fboID );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo.fboID );
    // The textures we're going to render to
    glGenTextures( num, fbo.colorID );
	for ( uint i = 0; i < num; i++ )
	{
		glBindTexture( GL_TEXTURE_2D, fbo.colorID[i] );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight,
				0, GL_RGBA, GL_FLOAT, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D
						, *(fbo.colorID + i), 0 );
	}

    // Depth texture
    glGenTextures( 1, &fbo.depthID );
    glBindTexture( GL_TEXTURE_2D, fbo.depthID );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo.depthID, 0 );

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	framebuffer[ fbo.fboID ] = fbo;
	return fbo.fboID;
}

void GBuffer::BindForWriting( uint fb )
{
	FBObj &fbo = framebuffer[fb];
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fb );
    GLenum drawBuffers[fbo.numTex + 1];
    for ( uint i = 0; i <= fbo.numTex; ++i )
    	drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers( fbo.numTex, drawBuffers );
}


void GBuffer::BindForReading( uint fb )
{
    glBindFramebuffer( GL_READ_FRAMEBUFFER, fb );
}


void GBuffer::BindTextures( uint fb )
{
	FBObj &fbo = framebuffer[fb];
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, fbo.depthID );
	for ( uint i = 0; i<=fbo.numTex; ++i )
	{
		glActiveTexture( GL_TEXTURE1 + i );
		glBindTexture( GL_TEXTURE_2D, fbo.colorID[i] );
	}
//	cout << textures[GBUFFER_TEXTURE_TYPE_DIFFUSE] << " " << textures[GBUFFER_TEXTURE_TYPE_NORMAL] << " " << depthTexture << endl;
}


