/**************************************************************************//**
 * @class Geometry
 * @file Geometry.h
 * @brief ...
 *  Created on: 14/02/2015
 *      Author: vivichrist
 *****************************************************************************/

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

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
	VertLoc, NormalLoc, TexCoordsLoc, ColourLoc, PositionLoc
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
	GLuint vao = 0, bytesSize, vbo, texture = 0;
	GLsizei numElements;
};

struct EBuffer
{
	GLenum vBuffType, eBuffType, type;
	GLuint vbo, ebo, vao = 0
			, vBytesSize, eBytesSize, texture = 0;
	GLsizei vNumElements, eNumElements;
};

class Geometry
{
public:
	Geometry();
	uint addSmoothSurfaceBuffer( vogl::Loader& );
	uint addBuffer( const std::string& );
	void bindTexure( const std::string&, GLuint );
	void draw( uint, GLsizei );
	void draw( uint[], GLsizei[] );
	void drawAll();
	virtual ~Geometry();
private:
	vogl::Texture tex;
	std::map< GLuint, Buffer > m_buffOb; // for vertices etc...
	std::map< GLuint, EBuffer > m_elemBuffOb; // for indices etc...
	/**< @class Geometry */
};

} /**< namespace vogl */

#endif /* GEOMETRY_HPP_ */
