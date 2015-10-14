/**************************************************************************//**
 * @class Geometry
 * @file Geometry.h
 * @brief ...
 *  Created on: 14/02/2015
 *      Author: vivichrist
 *****************************************************************************/

#pragma once
#define GLM_FORCE_RADIANS
#include <map>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Loader.hpp"
#include "Texture.hpp"

namespace vogl
{

enum LOC
{
	VertLoc, NormalLoc, TexCoordsLoc
	, PositionLoc, ColourLoc // instance data
};

struct Varying
{
	float vertex[3];
	float VNs[3];
	float UVs[2];
};

struct Buffer
{

	GLenum buffType, type;
	GLuint vao = 0, vbo;
	GLsizei numElements;
	GLuint64 bytesSize;
};

struct EBuffer
{
	GLenum vBuffType, eBuffType, type;
	GLuint vbo, ebo, vao = 0;
	GLsizei vNumElements, eNumElements;
	GLuint64 vBytesSize, eBytesSize;
};

class Geometry
{
public:
	static Geometry *getInstance();
	uint addSmoothSurfaceBuffer( const std::string&, const float*, const float*, uint );
	uint addBuffer( const std::string& load );
	uint addBuffer( const std::string& load, const glm::vec3& pos );
	uint addBuffer( const std::string& load, const glm::vec3& pos, const glm::vec3& col );
	uint addBuffer( const std::string&, const float*, const float*, uint );
	uint bindTexure( const std::string&, GLuint );
	uint bindCMTexure( const std::string&, GLuint );
	uint bindNMTexure( const std::string&, GLuint );
	void bindCMTexure( GLuint, GLuint );
	void bindNMTexure( GLuint, GLuint );
	void draw( uint, GLsizei );
	void draw( uint[], GLsizei[] );
	void drawAll();
	virtual ~Geometry();
private:
	Geometry();
	static Geometry *instance;
	std::map< GLuint, Buffer > m_buffOb; // for vertices etc...
	std::map< GLuint, EBuffer > m_elemBuffOb; // for indices etc...
	/**< @class Geometry */
};

} /**< namespace vogl */
