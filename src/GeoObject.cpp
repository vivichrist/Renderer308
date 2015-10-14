/**************************************************************************//**
 * @class GeoObject
 * @file GeoObject.cpp
 * @brief ...
 *  Created on: 14/10/2015
 *      Author: vivichrist
 *****************************************************************************/

#include "GeoObject.hpp"

namespace vogl
{

GeoObject::GeoObject( const std::string& file, const glm::vec3& pos )
	: ambspec{0}, blend(0), diffuse(0), reflection(0), amb(false), spec(false), cube(false), norm(false)
{
	Geometry *geo = Geometry::getInstance();
	geoID = geo->addBuffer( file, pos );
}

GeoObject::GeoObject( const std::string& file, const glm::vec3& pos,
		const glm::vec3& col )
	: ambspec{0}, blend(0), diffuse(0), reflection(0), amb(false), spec(false), cube(false), norm(false)
{
	Geometry *geo = Geometry::getInstance();
	geoID = geo->addBuffer( file, pos, col );
	materialDiffuse( col.r, col.g, col.b );
}

void GeoObject::drawObject( Shader& shader )
{
	// align textures to texture units
	for ( auto &t: textureUMap )
	{
		glActiveTexture( textureTypes[t.first] );
		glBindTexture( t.second, t.first );
	}

	// upload uniforms for this object
	if (amb) glUniform4fv( shader( "matAmb" ), 1, &ambspec[0] );
	if (spec) glUniform4fv( shader( "matSpec" ), 1, &ambspec[4] );
	if (cube) glUniform1f( shader( "matCubemap" ), ambspec[3] );
	if (norm) glUniform1f( shader( "matNormal" ), blend );
	Geometry *geo = Geometry::getInstance();
	geo->draw( geoID, 1 );

	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	glBindTexture( GL_NORMAL_MAP, 0 );
	glActiveTexture( GL_TEXTURE0 );
}

void GeoObject::materialAmb( float r, float g, float b )
{
	ambspec[0] = r;
	ambspec[1] = g;
	ambspec[2] = b;
	amb = true;
}

void GeoObject::materialReflect( float ref )
{
	ambspec[3] = ref;
}

void GeoObject::materialSpec( float r, float g, float b )
{
	ambspec[4] = r;
	ambspec[5] = g;
	ambspec[6] = b;
	spec = true;
}

void GeoObject::materialShininess( float s )
{
	ambspec[7] = s;
}

void GeoObject::materialDiffuse( float r, float g, float b )
{
	Texture *txt = Texture::getInstance();
	diffuse = glm::vec4( r,b,g,1 );
	uint tex = txt->addTexture( glm::vec3( diffuse ) );
	textureUMap[ tex ] = GL_TEXTURE0;
	textureTypes[ tex ] = GL_TEXTURE_2D;
}

void GeoObject::reflect( uint resolution, GLenum activeTU )
{
	Texture *txt = Texture::getInstance();
	reflection = txt->setupEnvMap( resolution );
	textureUMap[ reflection ] = activeTU;
	textureTypes[ reflection ] = GL_TEXTURE_CUBE_MAP;
}

void GeoObject::textureBlend( float b )
{
	blend = b;
	norm = true;
}

GeoObject::~GeoObject()
{

}

uint GeoObject::getReflectID()
{
	return reflection;
}

void GeoObject::texture( const std::string& file, GLenum type, GLenum activeTU )
{
	Texture *txt = Texture::getInstance();
	if ( type == GL_TEXTURE_2D )
	{
		uint tex = txt->addTexture( file );
		textureUMap[ tex ] = activeTU;
		textureTypes[ tex ] = type;
	}
	else if ( type == GL_TEXTURE_CUBE_MAP )
	{
		uint tex = txt->addCMTexture( file );
		textureUMap[ tex ] = activeTU;
		textureTypes[ tex ] = type;
		cube = true;
	}
	else if ( type == GL_NORMAL_MAP )
	{
		uint tex = txt->addNMTexture( file );
		textureUMap[ tex ] = activeTU;
		textureTypes[ tex ] = type;
		norm = true;
	}
}

} /**< namespace vogl */
