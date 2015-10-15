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
	, PositionLoc, ColourLoc,TangentLoc, BitangentLoc // instance data
};

struct Varying
{
	float vertex[3];
	float VNs[3];
	float UVs[2];
	float UVs2[2];
	float tangents[3];
	float bitangents[3];
};

struct Buffer
{

	GLenum buffType, type;
	GLuint vao = 0, vbo, texture = 0, cubeMap = 0, normalMap = 0, heightMap = 0;
	GLsizei numElements;
	GLuint64 bytesSize;
};

struct EBuffer
{
	GLenum vBuffType, eBuffType, type;
	GLuint vbo, ebo, vao = 0, texture = 0, cubeMap = 0, normalMap = 0, heightMap = 0;
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
	void computeTangentBasis( std::vector<glm::vec3> v,
			std::vector<glm::vec3> u, std::vector<glm::vec3> n, std::vector<glm::vec3> *t,std::vector<glm::vec3> *b);
	void bindTexure( const std::string&, GLuint );
	void bindCMTexure( const std::string&, GLuint );
	void bindNMTexure( const std::string&, GLuint );
	void bindHMTexure( const std::string&, GLuint );
	void bindCMTexure( GLuint, GLuint );
	void bindNMTexure( GLuint, GLuint );
	void bindHMTexure( GLuint, GLuint );
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
