/*
 * Texture.cpp
 *
 *  Created on: 24/08/2015
 *      Author: stewarvivi
 */

#include "Texture.hpp"

#define TEXTURE_LOAD_ERROR 0

using namespace std;
using namespace glm;

int checkGLError2( int where )
{
  int errCount = 0;
  for(GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError())
  {
  cout << "Error: " << currError << " line " << where <<  " In Texture\n";
    ++errCount;
  }
//  1280 GL_INVALID_ENUM
//  1281 GL_INVALID_VALUE
//  1282 GL_INVALID_OPERATION
//  1283 GL_STACK_OVERFLOW
//  1284 GL_STACK_UNDERFLOW
//  1285 GL_OUT_OF_MEMORY
  return errCount;
}

namespace vogl
{

Texture *Texture::instance = nullptr;

Texture::Texture()
{
}

Texture *Texture::getInstance()
{
	if ( instance == nullptr )
		instance = new Texture();
	return instance;
}

GLuint Texture::getPNGName( const string& name )
{
  return names[name];
}


GLuint Texture::addTexture( const string& filename )
{

	//Now generate the OpenGL texture object
	image tex( filename );
	GLuint texture;

	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex.w, tex.h, 0,
			tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) tex.data.data() );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glGenerateMipmap( GL_TEXTURE_2D );

	names[filename] = texture;
	return texture;
}

GLuint Texture::addTexture( const vec3& colour )
{

	//Now generate the OpenGL texture object
	GLuint texture;
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, 1, 1, 0, GL_RGB, GL_FLOAT,
			(GLvoid*) value_ptr( colour ) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	names[ to_string( texture ) ] = texture;
	return texture;
}

GLuint Texture::addCMTexture( const string& filename )
{
	image tex( filename );
	checkGLError2( 92 );
	EMap e;
	e.res = tex.w;
	//Now generate the OpenGL texture object
	glGenTextures( 1, &e.colorCMID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.colorCMID );
	//set texture parameters
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	uint dx = tex.w / 4, dy = tex.h / 3;
	image img = tex.subImage( dx * 2, dy, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	img = tex.subImage( 0, dy, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	img = tex.subImage( dx, 0, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	img = tex.subImage( dx, dy * 2, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	img = tex.subImage( dx, dy, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	img = tex.subImage( dx * 3, dy, dx, dy );
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,
		GL_RGB, dx, dy, 0, tex.glFormat(), GL_UNSIGNED_BYTE, (GLvoid*) img.data.data() );
	checkGLError2( 128 );
	e.res = dx;
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	checkGLError2( 131 );
	// glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	envir[e.colorCMID] = e;
	glActiveTexture( GL_TEXTURE0 );
	return e.colorCMID;
}

void Texture::activateTextures( uint fbID )
{
	FBObj& fbo = framebuffer[fbID];
	glBindFramebuffer( GL_FRAMEBUFFER, 0 ); // just in case
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID1 );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID2 );
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID3 );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID4 );
}

void Texture::activateFrameBuffer( uint fbID )
{
	glBindFramebuffer( GL_FRAMEBUFFER, fbID );
	GLenum drawBuffers[] =
	{
		  GL_COLOR_ATTACHMENT0
		, GL_COLOR_ATTACHMENT1
		, GL_COLOR_ATTACHMENT2
		, GL_COLOR_ATTACHMENT3
		, GL_COLOR_ATTACHMENT4
	};
	glDrawBuffers(5, drawBuffers);
}

GLuint Texture::setupFBO( uint width, uint height )
{
	FBObj fbo;
	glGenFramebuffers( 1, &fbo.fboID );

	// Generate and bind the texture for the depth buffer
	glGenTextures( 1, &fbo.depthID );
	glBindTexture( GL_TEXTURE_2D, fbo.depthID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0
			, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Generate and bind the texture for diffuse
	glGenTextures( 1, &fbo.colorID0 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID0 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA
			, GL_UNSIGNED_BYTE, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Generate and bind the texture for fragment normals
	glGenTextures( 1, &fbo.colorID1 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA
			, GL_FLOAT, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Generate and bind the texture for texture coordinates
	glGenTextures( 1, &fbo.colorID2 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID2 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA
			, GL_UNSIGNED_BYTE, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Generate and bind the texture for eye positions
	glGenTextures( 1, &fbo.colorID3 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID3 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA
			, GL_FLOAT,	0 );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Generate and bind the texture for reflection normals
	glGenTextures( 1, &fbo.colorID4 );
	glBindTexture( GL_TEXTURE_2D, fbo.colorID4 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA
			, GL_UNSIGNED_BYTE, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Bind the FBO so that the next operations will be bound to it.
	glBindFramebuffer( GL_FRAMEBUFFER, fbo.fboID );
	// Attach the texture to the FBO
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D
			, fbo.depthID, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D
			, fbo.colorID0, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D
			, fbo.colorID1, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D
			, fbo.colorID2, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D
			, fbo.colorID3, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D
			, fbo.colorID4, 0 );

	GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		printf( "DeferredLighting::Init: FrameBuffer incomplete: 0x%x\n",
				fboStatus );
		exit( 1 );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	framebuffer[ fbo.fboID ] = fbo;
	return fbo.fboID;
}

// in geometry shader version
GLuint Texture::setupEnvMap( uint resolution )
{
	EMap e;
	e.res = resolution;
	// how many textures to create? depends on complexity of your effects.
	glGenFramebuffers(1, &e.fboID);
	glGenTextures(1, &e.colorCMID );
	glGenTextures(1, &e.depthCMID );

	GLenum target = GL_TEXTURE_CUBE_MAP;

	// initialise depth map
	glBindTexture(target, e.depthCMID);
	for(int face = 0; face < 6; ++face)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					 GL_DEPTH_COMPONENT24, e.res, e.res, 0,
					 GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// setting up texture parameters
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	checkGLError2( 279 );


	// initialise colour map
	glBindTexture(target, e.colorCMID);
	for(int face = 0; face < 6; ++face)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					 GL_RGBA, e.res, e.res, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	checkGLError2( 293 );

	glBindFramebuffer(GL_FRAMEBUFFER, e.fboID);
	// attachments to the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT
			, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, e.depthCMID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0
			, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, e.colorCMID, 0);
	// unbind, back to the usual frame buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	envir[e.colorCMID] = e;
	return e.colorCMID;
}

void Texture::createOmniView( const vec3& position, mat4 mv[6], mat3 norm[6] )
{
	mv[0] = lookAt( vec3( 0 ), vec3( 1, 0, 0 ), vec3( 0,-1, 0 ) );
	mv[1] = lookAt( vec3( 0 ), vec3(-1, 0, 0 ), vec3( 0,-1, 0 ) );
	mv[2] = lookAt( vec3( 0 ), vec3( 0, 1, 0 ), vec3( 1, 0, 0 ) );
	mv[3] = lookAt( vec3( 0 ), vec3( 0,-1, 0 ), vec3( 1, 0, 0 ) );
	mv[4] = lookAt( vec3( 0 ), vec3( 0, 0, 1 ), vec3( 0,-1, 0 ) );
	mv[5] = lookAt( vec3( 0 ), vec3( 0, 0,-1 ), vec3( 0,-1, 0 ) );
	for ( int i = 0; i<6; ++i )
	{
	  mv[i] = glm::translate( mv[i], position );
	}
	norm[0] = inverse( transpose( mat3( mv[0] ) ) );
	norm[1] = inverse( transpose( mat3( mv[1] ) ) );
	norm[2] = inverse( transpose( mat3( mv[2] ) ) );
	norm[3] = inverse( transpose( mat3( mv[3] ) ) );
	norm[4] = inverse( transpose( mat3( mv[4] ) ) );
	norm[5] = inverse( transpose( mat3( mv[5] ) ) );
}

// render an cubemap the reflections
void Texture::useEnvironmentMap( Shader& shader, glm::vec3 position, const uint name )
{
	EMap& e = envir[name];

	//set the virtual viewer at the reflective object centre and render the scene
	mat4 views[6];
	mat3 nm[6];
	createOmniView( position, views, nm );

	//set the camera transform, 90.0 degrees FOV
	mat4 Pcubemap = perspective( (float) M_PI_2, 1.0f, 0.1f, 1000.0f );
	checkGLError2( 345 );
	//bind the FBO
	glBindFramebuffer( GL_FRAMEBUFFER, e.fboID );
	checkGLError2( 348 );
	shader.use();
	checkGLError2( 350 );
	//set the viewport to the size of the cube map texture
	glViewport( 0, 0, e.res, e.res );

	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	checkGLError2( 356 );
	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBuffers);
	checkGLError2( 359 );
	for ( uint i = 0; i<6; ++i )
	{
		glUniformMatrix4fv( shader( "mvM[0]" ) + i, 1, GL_FALSE,
				value_ptr( views[i] ) );
	}
	checkGLError2( 365 );
	for ( uint i = 0; i<6; ++i )
	{
		glUniformMatrix3fv( shader( "normM[0]" ) + i, 1, GL_FALSE,
				value_ptr( nm[i] ) );
	}
	checkGLError2( 371 );
	//using the cube map projection matrix and appropriate viewing settings
	glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
		value_ptr( Pcubemap ) );
	checkGLError2( 375 );
}

void Texture::unUseEnvironmentMap( Shader& shader, uint width, uint height, const uint name )
{
	EMap& e = envir[name];

	//unbind the FBO
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	checkGLError2( 379 );
	//reset the default viewport
	glViewport( 0, 0, width, height );
	checkGLError2( 382 );
	shader.unUse();
}

Texture::~Texture()
{
	for ( auto n : names )
	{
		glDeleteTextures( 1, &n.second );
	}
	for ( auto e : envir )
	{
		glDeleteTextures( 1, &e.second.depthCMID );
		glDeleteTextures( 1, &e.second.colorCMID );
		glDeleteFramebuffers( 1, &e.second.fboID );
	}
}

} /* namespace vogl */
