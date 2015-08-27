/**************************************************************************//**
 * @class Geometry
 * @file Geometry.cpp
 * @brief ...
 *  Created on: 14/02/2015
 *      Author: vivichrist
 *****************************************************************************/

#include "Geometry.hpp"

using namespace std;
using namespace glm;

namespace vogl
{

Geometry::Geometry()
{
}

int checkGLError( int where )
{
  int errCount = 0;
  for(GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError())
  {
	cout << "Error: " << currError << " line " << where <<  " In Geometry\n";
    ++errCount;
  }

  return errCount;
}

Geometry::~Geometry()
{
	for ( auto p : m_buffOb )
	{
		glDeleteVertexArrays( 1, &p.second.vao );
		glDeleteBuffers( 1, &p.second.vbo );
	}
	for ( auto p : m_elemBuffOb )
	{
		glDeleteVertexArrays( 1, &p.second.vao );
		glDeleteBuffers( 1, &p.second.vbo );
		glDeleteBuffers( 1, &p.second.ebo );
	}
}
/******************************************************************************
 * Fill a buffer with Vertices, Texture Coorinates and Vertex Normals ready for
 * rendering. We assume here that normals texture coodinates will be the same
 * for each point and thus can index them. Buffers are sent straight to VRAM.
 * @param triangles straight from the loader
 * @return the VAO name
 */
uint Geometry::addSmoothSurfaceBuffer( Loader& triangles )
{
	vector<vec3> v;
	triangles.getPoints( v );
	vector<vec2> vt;
	triangles.getUVs( vt );
	vector<vec3> vn;
	triangles.getNormals( vn );
	vector<triangle> tris;
	triangles.getTriIndices( tris );
	ushort max = v.size();
	if ( !max )
	{
		cout << "No Vertices To Load!";
		throw;
	}
	map<int, Varying*> mbuff;
	vector<Varying> buff(max);
	vector<GLushort> indices;
	for ( triangle t: tris )
	{ // load in the location data each possibly empty except points
		for ( uint j = 0; j<3; ++j )
		{
			indices.push_back( t.v[j].p );
			if ( mbuff.find(t.v[j].p) != mbuff.end() ) continue;
			Varying b;
			vertex &k = t.v[j];
			b.vertex[0] = v[k.p].x;
			b.vertex[1] = v[k.p].y;
			b.vertex[2] = v[k.p].z;
			// vertex normals
			b.VNs[0] = vn[k.n].x;
			b.VNs[1] = vn[k.n].y;
			b.VNs[2] = vn[k.n].z;
			// texture coodinates
			b.UVs[0] = vt[k.t].x;
			b.UVs[1] = vt[k.t].y;
			buff[t.v[j].p] = b;
			mbuff[t.v[j].p] = &(buff.data()[t.v[j].p]);
		}
	}
	EBuffer b;
	b.vNumElements = max;
	b.vBytesSize = sizeof(Varying) * max;
	b.vBuffType = GL_FLOAT;
	b.type = GL_ARRAY_BUFFER;
	checkGLError( 102 );
	// create a new vertex array object
	glGenVertexArrays( 1, &b.vao );
	glBindVertexArray( b.vao );
	// initialise/bind vertex buffer object
	glGenBuffers( 1, &b.vbo );
	glBindVertexArray( b.vbo );
	glBindBuffer( b.type, b.vbo );
	checkGLError( 110 );
	// write data
	glBufferData( b.type, b.vBytesSize, &(buff.data()[0]), GL_STATIC_DRAW );
	checkGLError( 113 );

	// set locations for the shader
	glVertexAttribPointer( VertLoc, 3, b.vBuffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) 0 );
	glEnableVertexAttribArray( VertLoc );
	glVertexAttribPointer( NormalLoc, 3, b.vBuffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) (sizeof(float) * 3) );
	glEnableVertexAttribArray( NormalLoc );
	glVertexAttribPointer( TexCoordsLoc, 2, b.vBuffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) (sizeof(float) * 6) );
	glEnableVertexAttribArray( TexCoordsLoc );
	checkGLError( 131 );
	glVertexAttribDivisor( ColourLoc, 1 );
	glVertexAttribDivisor( PositionLoc, 1 );
	checkGLError( 134 );
	// and then indices (element buffer)
	auto data = indices.data();
	b.eNumElements = indices.size();
	b.eBytesSize = sizeof(GLushort) * b.eNumElements;
	b.eBuffType = GL_UNSIGNED_SHORT;
	b.type = GL_ELEMENT_ARRAY_BUFFER;
	glGenBuffers( 1, &b.ebo );
	glBindBuffer( b.type, b.ebo );
	glBufferData( b.type, b.eBytesSize, (GLvoid *) &data[0], GL_STATIC_DRAW );
	checkGLError( 144 );
	// the VAO should remember all of that
	m_elemBuffOb[b.vao] = b;
	return b.vao;
}

void Geometry::bindTexure( const std::string& load, GLuint id )
{
	int h = 512, w = 512;
	GLuint texture = tex.addPNGTexture( load, h, w );
	if ( m_elemBuffOb.find( id ) != m_elemBuffOb.end() )
	{
		m_elemBuffOb[id].texture = texture;
	}
	else if ( m_buffOb.find( id ) != m_buffOb.end() )
	{
		m_buffOb[id].texture = texture;
	}
	else
	{
		std::cout << "No Such VBObject (name:" << id << ")\n";
		throw;
	}
}

/******************************************************************************
 * Fill a buffer with Vertices, Texture Coorinates and Vertex Normals ready for
 * rendering.
 * @param triangles straight from the loader
 * @return the VAO name
 */
uint Geometry::addBuffer( const string& load )
{
	vogl::Loader triangles;
	triangles.readOBJ( load );

	vector<vec3> v;
	triangles.getPoints( v );
	vector<vec2> vt;
	triangles.getUVs( vt );
	vector<vec3> vn;
	triangles.getNormals( vn );
	vector<triangle> tris;
	triangles.getTriIndices( tris );
	ushort max = tris.size() * 3;
	if ( !max )
	{
		cout << "No Vertices To Load!";
		throw;
	}
	Varying buff[max];
	uint i = 0;
	for ( triangle t: tris )
	{ // load in the location data each possibly empty except points
		for ( uint j = 0; j<3; ++j )
		{
			Varying b;
			vertex &k = t.v[j];
			b.vertex[0] = v[k.p].x;
			b.vertex[1] = v[k.p].y;
			b.vertex[2] = v[k.p].z;
			// vertex normals
			b.VNs[0] = vn[k.n].x;
			b.VNs[1] = vn[k.n].y;
			b.VNs[2] = vn[k.n].z;
			// texture coordinates
			b.UVs[0] = vt[k.t].x;
			b.UVs[1] = vt[k.t].y;
			buff[i + j] = b;
		}
		i += 3;
	}
	Buffer b;
	b.numElements = max;
	b.bytesSize = sizeof(Varying) * max;
	b.buffType = GL_FLOAT;
	b.type = GL_ARRAY_BUFFER;
	glGenVertexArrays( 1, &b.vao );
	glBindVertexArray( b.vao );
	glGenBuffers( 1, &b.vbo );
	glBindBuffer( b.type, b.vbo );
	glBufferData( b.type, b.bytesSize, &buff[0], GL_STATIC_DRAW );
	checkGLError( 208 );
	glVertexAttribPointer( VertLoc, 3, b.buffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) 0 );
	glEnableVertexAttribArray( VertLoc );
	glVertexAttribPointer( NormalLoc, 3, b.buffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) (sizeof(float) * 3) );
	glEnableVertexAttribArray( NormalLoc );
	glVertexAttribPointer( TexCoordsLoc, 2, b.buffType, GL_FALSE
			, sizeof(Varying), (GLvoid *) (sizeof(float) * 6) );
	glEnableVertexAttribArray( TexCoordsLoc );
	checkGLError( 227 );
	m_buffOb[b.vao] = b;
	return b.vao;
}
/******************************************************************************
 * Draw the contents of a buffer(s) on the video card
 * @param id opengl reference to the vertex array object
 * @param insts howmany instances we will draw
 * TODO: create buffers for instances
 */
void Geometry::draw( uint id, GLsizei insts )
{
	EBuffer e;
	Buffer b;
	if ( m_elemBuffOb.find( id ) != m_elemBuffOb.end() )
	{
		e = m_elemBuffOb[id];
		glBindTexture( GL_TEXTURE_2D, e.texture );
		glBindVertexArray( e.vao );
		if ( insts == 1 )
			glDrawElements( GL_TRIANGLES, e.eNumElements, e.eBuffType,
					(GLvoid *) 0 );
		else
			glDrawElementsInstanced( GL_TRIANGLES, e.eNumElements, e.eBuffType,
					(GLvoid *) 0, insts );
		checkGLError( 246 );
	}
	else if ( m_buffOb.find( id ) != m_buffOb.end() )
	{
		b = m_buffOb[id];
		glBindTexture( GL_TEXTURE_2D, b.texture );
		glBindVertexArray( b.vao );
		if ( insts == 1 )
			glDrawArrays( GL_TRIANGLES, 0, b.numElements );
		else
			glDrawArraysInstanced( GL_TRIANGLES, 0, b.numElements, insts );
		checkGLError( 255 );
	}
	else
	{
		std::cout << "No Such Vertex Array Object (name:" << id << ")\n";
		throw;
	}
}

void Geometry::draw( uint id[], GLsizei insts[] )
{
  // TODO:
}

void Geometry::drawAll()
{
  // TODO
}

} /**< namespace vogl */
