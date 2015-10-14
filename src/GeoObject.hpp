/**************************************************************************//**
 * @class GeoObject
 * @file GeoObject.h
 * @brief ...
 *  Created on: 14/10/2015
 *      Author: vivichrist
 *****************************************************************************/

#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include "Geometry.hpp"
#include "Texture.hpp"

namespace vogl
{

class GeoObject
{

public:
	GeoObject( const std::string& file, const glm::vec3& pos, const glm::vec3& col );
	GeoObject( const std::string& file, const glm::vec3& pos );
	virtual ~GeoObject();
	void drawObject( Shader& );
	void materialAmb( float r, float g, float b );
	void materialReflect( float ref );
	void materialSpec( float r, float g, float b );
	void materialShininess( float s );
	void materialDiffuse( float r, float g, float b );
	void textureBlend( float b );
	void reflect( uint resolution, GLenum activeTU );
	uint getReflectID();
	void texture( const std::string& file, GLenum type, GLenum activeTU );
private:
	float ambspec[8];
	float blend;
	glm::vec4 diffuse;
	uint geoID, reflection;
	bool amb, spec, cube, norm;
	// texture mapping from opengl texture name to texture unit
	std::map<uint, GLenum> textureUMap;
	std::map<uint, GLenum> textureTypes;
};

} /**< namespace vogl */
