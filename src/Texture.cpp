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
	glActiveTexture( GL_TEXTURE0 );
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
	glActiveTexture( GL_TEXTURE0 );
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
	glActiveTexture( GL_TEXTURE1 );
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
	envir[filename] = e;
	checkGLError2( 132 );
	return e.colorCMID;
}

GLuint Texture::addNMTexture( const string& filename )
{

	//Load normal map
	checkGLError2( 133 );
	image normalMapImage( filename );

	//Convert normal map to texture
	GLuint normalMap;
	glGenTextures(1, &normalMap);
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, normalMapImage.w, normalMapImage.h,
		0, normalMapImage.glFormat(), GL_UNSIGNED_BYTE, normalMapImage.data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	checkGLError2( 134 );

	return normalMap;
}

GLuint Texture::addHMTexture( const string& filename )
{

	//Load normal map
	checkGLError2( 134 );
	image normalMapImage( filename );

	//Convert normal map to texture
	GLuint heightMap;
	glGenTextures(1, &heightMap);
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture(GL_TEXTURE_2D, heightMap);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, normalMapImage.w, normalMapImage.h,
		0, normalMapImage.glFormat(), GL_UNSIGNED_BYTE, normalMapImage.data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	checkGLError2( 134 );

	return heightMap;
}

GLuint Texture::getEnvMap( const string& name )
{
	EMap& e = envir[name];
	return e.colorCMID;
}

// in geometry shader version
GLuint Texture::setupEnvMap( const string& name, uint resolution )
{
	EMap e;
	e.res = resolution;
	// how many textures to create? depends on complexity of your effects.
	glGenFramebuffers(1, &e.fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, e.fboID);
	glGenTextures(1, &e.colorCMID );
	glGenTextures(1, &e.depthCMID );

	glActiveTexture( GL_TEXTURE1 );
	GLenum target = GL_TEXTURE_CUBE_MAP;

	// initialise depth map
	glBindTexture(target, e.depthCMID);
	// setting up texture parameters
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_COMPARE_MODE,
							GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);

	for(int face = 0; face < 6; ++face)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					 GL_DEPTH_COMPONENT24, e.res, e.res, 0,
					 GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// attaching to the frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						 e.depthCMID, 0);

	// initialise colour map
	glBindTexture(target, e.colorCMID);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for(int face = 0; face < 6; ++face)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					 GL_RGBA32F, e.res, e.res, 0,
					 GL_RGBA, GL_FLOAT, 0);

	// attaching to the frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			e.colorCMID, 0);

	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};//, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture( GL_TEXTURE0 );

	e.shader.loadFromFile( GL_VERTEX_SHADER, "vertex_cmap.glsl" );
	e.shader.loadFromFile( GL_GEOMETRY_SHADER, "geometry_cmap.glsl" );
	e.shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_cmap.glsl" );
	e.shader.createAndLinkProgram();
	e.shader.use();
		e.shader.addUniform( "mvM[0]" );
		e.shader.addUniform( "projM" );
		e.shader.addUniform( "normM[0]" );
		e.shader.addUniform( "matAmb" );
		e.shader.addUniform( "matSpec" );
		e.shader.addUniform( "numLights" );
		e.shader.addUniform( "allLights[0]" );
		e.shader.addUniform( "image" );
		e.shader.addUniform( "normalmap" );
	e.shader.unUse();

	e.shader.printActiveUniforms();

	envir[name] = e;
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
void Texture::useEnvironmentMap( Shader& shader, glm::vec3 position, const string& name )
{
	EMap& e = envir[name];

	//set the virtual viewer at the reflective object centre and render the scene
	mat4 views[6];
	mat3 nm[6];
	createOmniView( position, views, nm );

	//set the camera transform, 90.0 degrees FOV
	mat4 Pcubemap = perspective( (float) M_PI_2, 1.0f, 0.1f, 1000.0f );

	//bind the FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, e.fboID );
	glActiveTexture( GL_TEXTURE1 );

	e.shader.use();

	checkGLError2( 272 );
	//set the viewport to the size of the cube map texture
	glViewport( 0, 0, e.res, e.res );

	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for ( uint i = 0; i<6; ++i )
	{
		glUniformMatrix4fv( e.shader( "mvM[0]" ) + i, 1, GL_FALSE,
				value_ptr( views[i] ) );
	}
	for ( uint i = 0; i<6; ++i )
	{
		glUniformMatrix3fv( e.shader( "normM[0]" ) + i, 1, GL_FALSE,
				value_ptr( nm[i] ) );
	}
	checkGLError2( 289 );
	//using the cube map projection matrix and appropriate viewing settings
	glUniformMatrix4fv( e.shader( "projM" ), 1, GL_FALSE,
		value_ptr( Pcubemap ) );
	checkGLError2( 393 );
	shader = e.shader;
}

uint Texture::unUseEnvironmentMap( uint width, uint height, const string& name )
{
	EMap& e = envir[name];

	//unbind the FBO
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	checkGLError2( 351 );
	//reset the default viewport
	glViewport( 0, 0, width, height );
	checkGLError2( 354 );
	e.shader.unUse();
	glActiveTexture( GL_TEXTURE0 );
	return e.colorCMID;
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
