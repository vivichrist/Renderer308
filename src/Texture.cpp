/*
 * Texture.cpp
 *
 *  Created on: 24/08/2015
 *      Author: stewarvivi
 */

#include "Texture.hpp"

#define TEXTURE_LOAD_ERROR 0

using namespace std;

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

GLuint Texture::getPNGName( const std::string& allocator )
{
}

GLuint Texture::addPNGTexture( const string& filename, int &width, int &height )
{
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen( filename.c_str(), "rb" );
	if ( !fp )
	{
		return TEXTURE_LOAD_ERROR;
	}

	//read the header
	fread( header, 1, 8, fp );

	//test if png
	int is_png = !png_sig_cmp( header, 0, 8 );
	if ( !is_png )
	{
		fclose( fp );
		return TEXTURE_LOAD_ERROR;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL,
	NULL, NULL );
	if ( !png_ptr )
	{
		fclose( fp );
		return (TEXTURE_LOAD_ERROR);
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct( png_ptr );
	if ( !info_ptr )
	{
		png_destroy_read_struct( &png_ptr, (png_infopp) NULL,
				(png_infopp) NULL );
		fclose( fp );
		return (TEXTURE_LOAD_ERROR);
	}

	//create png info struct
	png_infop end_info = png_create_info_struct( png_ptr );
	if ( !end_info )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
		fclose( fp );
		return (TEXTURE_LOAD_ERROR);
	}

	//png error stuff, not sure libpng man suggests this.
	if ( setjmp( png_jmpbuf(png_ptr) ) )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
		fclose( fp );
		return (TEXTURE_LOAD_ERROR);
	}

	//init png reading
	png_init_io( png_ptr, fp );

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes( png_ptr, 8 );

	// read all the info up to the image data
	png_read_info( png_ptr, info_ptr );

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR( png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
	NULL, NULL, NULL );

	//update width and height based on png info
	width = twidth;
	height = theight;

	// Update the png info struct.
	png_read_update_info( png_ptr, info_ptr );

	// Row size in bytes.
	int rowbytes = png_get_rowbytes( png_ptr, info_ptr );

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if ( !image_data )
	{
		//clean up memory and close stuff
		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
		fclose( fp );
		return TEXTURE_LOAD_ERROR;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if ( !row_pointers )
	{
		//clean up memory and close stuff
		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
		delete[] image_data;
		fclose( fp );
		return TEXTURE_LOAD_ERROR;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for ( int i = 0; i < height; ++i )
		row_pointers[height - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image( png_ptr, row_pointers );

	//Now generate the OpenGL texture object
	GLuint texture;
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) image_data );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glGenerateMipmap( GL_TEXTURE_2D );
	//clean up memory and close stuff
	png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
	delete[] image_data;
	delete[] row_pointers;
	fclose( fp );

	names[filename] = texture;
	return texture;
}

//initialize FBO the hard way
void Texture::makeEnviromentMap( const string& filename, uint width,
		uint height, uint resolution )
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
void Texture::setupEnvMap( const string& name, uint width, uint height,
		uint resolution )
{
	EMap e;
	e.res = resolution;
	// depth cube map
	glGenTextures( 1, &e.depthCMID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.depthCMID );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	for ( uint face = 0; face < 6; face++ )
	{
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
				GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT,
				GL_FLOAT, nullptr );
	}

	// color cube map
	glGenTextures( 1, &e.colorCMID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, e.colorCMID );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	for ( uint face = 0; face < 6; face++ )
	{
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, width,
				height, 0, GL_RGBA, GL_FLOAT, nullptr );
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
}

void Texture::createOmniView( glm::vec3 position, glm::mat4 out[6] )
{
	out[0] = glm::lookAt( glm::vec3( 0 ), glm::vec3( 1, 0, 0 )
						, glm::vec3( 0, -1, 0 ) );
	out[1] = glm::lookAt( glm::vec3( 0 ), glm::vec3( -1, 0, 0 )
						, glm::vec3( 0, -1, 0 ) );
	out[2] = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 1, 0 )
						, glm::vec3( 1, 0, 0 ) );
	out[3] = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, -1, 0 )
						, glm::vec3( 1, 0, 0 ) );
	out[4] = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 0, 1 )
						, glm::vec3( 0, -1, 0 ) );
	out[5] = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 0, -1 )
						, glm::vec3( 0, -1, 0 ) );
	for ( glm::mat4& m: out )
	{
		m = glm::translate( m, position );
	}
}

// render an environment map the hard way TODO: actually this doesn't work yet...
void Texture::renderEnvironmentMap( glm::vec3 position, string& name )
{
	//clear colour buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	EMap e = envir[name];

	//set the camera transform
	glm::mat4 Pcubemap = glm::perspective( (float) M_PI_2, 1.0f, 0.1f,
			1000.0f ); // 90.0 degrees FOV

	//set the viewport to the size of the cube map texture
	glViewport( 0, 0, e.res, e.res );

	//bind the FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, e.fboID );

	//set the GL_TEXTURE_CUBE_MAP_POSITIVE_X to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV1 = glm::lookAt( glm::vec3( 0 ), glm::vec3( 1, 0, 0 ),
			glm::vec3( 0, -1, 0 ) );
	// DrawScene( MV1 * position, Pcubemap );

	//set the GL_TEXTURE_CUBE_MAP_NEGATIVE_X to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV2 = glm::lookAt( glm::vec3( 0 ), glm::vec3( -1, 0, 0 ),
			glm::vec3( 0, -1, 0 ) );
	// DrawScene( MV2 * position, Pcubemap );

	//set the GL_TEXTURE_CUBE_MAP_POSITIVE_Y to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV3 = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 1, 0 ),
			glm::vec3( 1, 0, 0 ) );
	// DrawScene( MV3 * position, Pcubemap );

	//set the GL_TEXTURE_CUBE_MAP_NEGATIVE_Y to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV4 = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, -1, 0 ),
			glm::vec3( 1, 0, 0 ) );
	// DrawScene( MV4 * position, Pcubemap );

	//set the GL_TEXTURE_CUBE_MAP_POSITIVE_Z to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV5 = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 0, 1 ),
			glm::vec3( 0, -1, 0 ) );
	// DrawScene( MV5 * position, Pcubemap );

	//set the GL_TEXTURE_CUBE_MAP_NEGATIVE_Z to the colour attachment of FBO
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, e.colorCMID, 0 );
	//clear the colour and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set the virtual viewrer at the reflective object center and render the scene
	//using the cube map projection matrix and appropriate viewing settings
	glm::mat4 MV6 = glm::lookAt( glm::vec3( 0 ), glm::vec3( 0, 0, -1 ),
			glm::vec3( 0, -1, 0 ) );
	// DrawScene( MV6 * position, Pcubemap );

	//unbind the FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

	//reset the default viewport
	// glViewport( 0, 0, WIDTH, HEIGHT );
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
