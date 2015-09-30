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
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, 1, 1, 0, GL_RGB
      , GL_FLOAT, (GLvoid*) value_ptr( colour ) );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  names[ to_string( texture ) ] = texture;
  return texture;
}

//initialize FBO the hard way
void Texture::makeEnviromentMap( const string& filename, uint resolution )
{
	EMap e;
	e.res = resolution;
	//generate the dynamic cubemap texture and bind to texture unit 1
	glGenTextures( 1, &e.colorCMID );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.colorCMID );
	//set texture parameters
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	//for all 6 cubemap faces
	for ( int face = 0; face < 6; face++ )
	{
		//allocate a different texture for each face and assign to the cubemap texture target
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
				resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL );
	}

	//setup FBO
	glGenFramebuffers( 1, &e.fboID );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, e.fboID );

	//setup render buffer object (RBO)
	glGenRenderbuffers( 1, &e.rbID );
	glBindRenderbuffer( GL_RENDERBUFFER, e.rbID );

	//set the render buffer storage to have the same dimensions as the cubemap texture
	//also set the render buffer as the depth attachment of the FBO
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution,
			resolution );
	glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_RENDERBUFFER, e.fboID );

	//set the dynamic cubemap texture as the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X, e.colorCMID, 0 );

	//check the framebuffer completeness status
	GLenum status = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		cerr << "Frame buffer object setup error." << endl;
		exit( EXIT_FAILURE );
	}
	else
	{
		cerr << "FBO setup successfully." << endl;
	}
	//unbind FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	//unbind renderbuffer
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	cout << "Initialization successfull" << endl;

	envir[filename] = e;
}
// in geometry shader version
GLuint Texture::setupEnvMap( const string& name, uint resolution )
{
	EMap e;
	e.res = resolution;
	// depth cube map
	glGenTextures( 1, &e.depthCMID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.depthCMID );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER
	      , GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	for ( uint face = 0; face < 6; face++ )
	{
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0
		    , GL_DEPTH_COMPONENT24, e.res, e.res, 0, GL_DEPTH_COMPONENT
		    , GL_FLOAT, nullptr );
	}

	// color cube map
	glGenTextures( 1, &e.colorCMID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.colorCMID );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER
	      , GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	for ( uint face = 0; face < 6; face++ )
	{
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA
		    , e.res, e.res, 0, GL_RGBA, GL_FLOAT, nullptr );
	}

	// framebuffer object
	glGenFramebuffers( 1, &e.fboID );
	glBindFramebuffer( GL_FRAMEBUFFER, e.fboID );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, e.depthCMID, 0 );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, e.colorCMID, 0 );

	glDrawBuffer( GL_COLOR_ATTACHMENT0 );

	GLenum status = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		cerr << "Frame buffer object setup error." << endl;

		exit( EXIT_FAILURE );
	}
	else
	{
		cerr << "FBO setup successfully." << endl;
	}
	//unbind FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	//unbind renderbuffer
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	envir[name] = e;
	return e.fboID;
}

void Texture::createOmniView( const vec3& position, mat4 out[6] )
{
	out[0] = lookAt( vec3( 0 ), vec3( 1, 0, 0 ), vec3( 0,-1, 0 ) );
	out[1] = lookAt( vec3( 0 ), vec3(-1, 0, 0 ), vec3( 0,-1, 0 ) );
	out[2] = lookAt( vec3( 0 ), vec3( 0, 1, 0 ), vec3( 1, 0, 0 ) );
	out[3] = lookAt( vec3( 0 ), vec3( 0,-1, 0 ), vec3( 1, 0, 0 ) );
	out[4] = lookAt( vec3( 0 ), vec3( 0, 0, 1 ), vec3( 0,-1, 0 ) );
	out[5] = lookAt( vec3( 0 ), vec3( 0, 0,-1 ), vec3( 0,-1, 0 ) );
	for ( int i = 0; i<6; ++i )
	{
	  out[i] = glm::translate( out[i], position );
	}
}

// render an environment map the hard way TODO: actually this doesn't work yet...
void Texture::useEnvironmentMap( glm::vec3 position, string& name, uint width, uint height )
{
	//clear colour buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	EMap e = envir[name];

	//
	mat4 views[6];
	createOmniView( position, views );

	//set the camera transform, 90.0 degrees FOV
	mat4 Pcubemap = perspective( (float) M_PI_2, 1.0f, 0.1f, 1000.0f );

	//set the viewport to the size of the cube map texture
	glViewport( 0, 0, e.res, e.res );

	Shader shader;
  shader.loadFromFile( GL_VERTEX_SHADER, "vertex_cmap.glsl" );
  shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_cmap.glsl" );
  shader.createAndLinkProgram();
  shader.use();
    shader.addUniform( "mvM" );
    shader.addUniform( "projM" );
    shader.addUniform( "normM" );
    shader.addUniform( "lightP" );
  shader.unUse();

	//bind the FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, e.fboID );

	for ( int i = 0; i<6; ++i )
	{
    //set the GL_TEXTURE_CUBE_MAP_POSITIVE_X to the colour attachment of FBO
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, e.colorCMID, 0 );
    //clear the colour and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //set the virtual viewrer at the reflective object center and render the scene
    //using the cube map projection matrix and appropriate viewing settings
    glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE,
        value_ptr( views[i] ) );
    glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
        value_ptr( Pcubemap ) );
    glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,
        value_ptr( glm::inverse( glm::transpose( views[i] ) ) ) );
    // glUniform3fv( shader( "lightP" ), 1, &lightP[0] );
    // geo->draw( name, 9 );
    // DrawScene( MV * position, Pcubemap );
	}
	shader.unUse();

	//unbind the FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

	//reset the default viewport
	glViewport( 0, 0, width, height );
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
		glDeleteRenderbuffers( 1, &e.second.rbID );
		glDeleteFramebuffers( 1, &e.second.fboID );
	}
}

} /* namespace vogl */
