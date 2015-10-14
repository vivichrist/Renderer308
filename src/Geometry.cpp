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

namespace vogl {

Geometry::Geometry() {
}

Geometry *Geometry::instance = nullptr;

Geometry *Geometry::getInstance() {
	if (instance == nullptr)
		instance = new Geometry();
	return instance;
}

int checkGLError(int where) {
	int errCount = 0;
	for (GLenum currError = glGetError(); currError != GL_NO_ERROR; currError =
			glGetError()) {
		cout << "Error: " << currError << " line " << where << " In Geometry\n";
		++errCount;
	}

	return errCount;
}

Geometry::~Geometry() {
	for (auto p : m_buffOb) {
		glDeleteVertexArrays(1, &p.second.vao);
		glDeleteBuffers(1, &p.second.vbo);
	}
	for (auto p : m_elemBuffOb) {
		glDeleteVertexArrays(1, &p.second.vao);
		glDeleteBuffers(1, &p.second.vbo);
		glDeleteBuffers(1, &p.second.ebo);
	}
	m_buffOb.clear();
	m_elemBuffOb.clear();
	instance = nullptr;
}
/******************************************************************************
 * Fill a buffer with Vertices, Texture Coorinates and Vertex Normals ready for
 * rendering. We assume here that normals texture coodinates will be the same
 * for each point and thus can index them. Buffers are sent straight to VRAM.
 * @param triangles straight from the loader
 * @return the VAO name
 * TODO: untested...
 */
uint Geometry::addSmoothSurfaceBuffer(const string& load, const float *pos,
		const float *col, uint n) {
	vogl::Loader triangles;
	triangles.readOBJ(load);
	// grab all relevant triangle info
	vector<vec3> v;
	triangles.getPoints(v);
	vector<vec2> vt;
	triangles.getUVs(vt);
	vector<vec3> vn;
	triangles.getNormals(vn);
	vector<triangle> tris;
	triangles.getTriIndices(tris);
	uint max = v.size();
	if (!max) {
		cout << "No Vertices To Load!";
		throw;
	}
	map<int, Varying*> mbuff;
	vector<Varying> buff(max);
	vector<GLuint> indices;
	for (triangle t : tris) { // load in the location data each possibly empty except points
		for (uint j = 0; j < 3; ++j) {
			indices.push_back(t.v[j].p);
			if (mbuff.find(t.v[j].p) != mbuff.end())
				continue;
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
	checkGLError(102);
	// create a new vertex array object
	glGenVertexArrays(1, &b.vao);
	glBindVertexArray(b.vao);
	// initialise/bind vertex buffer object
	glGenBuffers(1, &b.vbo);
	glBindVertexArray(b.vbo);
	glBindBuffer(b.type, b.vbo);
	checkGLError(110);
	// write data
	uint isize = sizeof(float) * 3 * n;
	glBufferData(b.type, b.vBytesSize + isize * 2, (GLvoid *) 0,
			GL_STATIC_DRAW);
	glBufferSubData(b.type, 0, b.vBytesSize, &(buff.data()[0]));
	checkGLError(113);

	// set locations for the shader
	glVertexAttribPointer(VertLoc, 3, b.vBuffType, GL_FALSE, sizeof(Varying),
			(GLvoid *) 0);
	glEnableVertexAttribArray(VertLoc);
	glVertexAttribPointer(NormalLoc, 3, b.vBuffType, GL_FALSE, sizeof(Varying),
			(GLvoid *) (sizeof(float) * 3));
	glEnableVertexAttribArray(NormalLoc);
	glVertexAttribPointer(TexCoordsLoc, 2, b.vBuffType, GL_FALSE,
			sizeof(Varying), (GLvoid *) (sizeof(float) * 6));
	glEnableVertexAttribArray(TexCoordsLoc);
	checkGLError(134);
	// instancing attributes
	if (n > 0) // bypass if there are none
			{
		uint fs = 2 * 3 * n, k = 3 * n;
		float poscol[fs];
		for (uint i = 0; i < fs; ++i)
			poscol[i] = (i < k) ? pos[i] : col[i - k];
		glBufferSubData(b.type, b.vBytesSize, isize * 2, &poscol[0]);
		glVertexAttribPointer(PositionLoc, 3, b.vBuffType, GL_FALSE, 0,
				(GLvoid *) b.vBytesSize);
		glEnableVertexAttribArray(PositionLoc);
		glVertexAttribDivisor(PositionLoc, 1);
		glVertexAttribPointer(ColourLoc, 3, b.vBuffType, GL_FALSE, 0,
				(GLvoid *) (b.vBytesSize + isize));
		glEnableVertexAttribArray(ColourLoc);
		glVertexAttribDivisor(ColourLoc, 1);
	}
	// and then indices (element buffer)
	auto data = indices.data();
	b.eNumElements = indices.size();
	b.eBytesSize = sizeof(GLuint) * b.eNumElements;
	b.eBuffType = GL_UNSIGNED_INT;
	b.type = GL_ELEMENT_ARRAY_BUFFER;
	glGenBuffers(1, &b.ebo);
	glBindBuffer(b.type, b.ebo);
	glBufferData(b.type, b.eBytesSize, (GLvoid *) &data[0], GL_STATIC_DRAW);
	checkGLError(144);
	// the VAO should remember all of that
	m_elemBuffOb[b.vao] = b;
	return b.vao;
}

uint Geometry::addBuffer(const string& load) {
	float p[] = { 0, 0, 0 };
	return addBuffer(load, p, p, 1);
}

uint Geometry::addBuffer(const string& load, const vec3& pos) {
	float p[] = { pos.x, pos.y, pos.z };
	return addBuffer(load, p, p, 1);
}

uint Geometry::addBuffer(const string& load, const vec3& pos, const vec3& col) {
	float p[] = { pos.x, pos.y, pos.z };
	float c[] = { col.x, col.y, col.z };
	return addBuffer(load, p, c, 1);
}

/******************************************************************************
 * Fill a buffer with Vertices, Texture Coordinates and Vertex Normals ready for
 * rendering.
 * @param load name of file to load triangles etc.
 * @param pos positions of a number of instances
 * @param col colour of a number of instances
 * @param n number of instances
 * @return the VAO name
 */
uint Geometry::addBuffer(const string& load, const float *pos, const float *col,
		uint n) {
	vogl::Loader triangles;
	triangles.readOBJ(load);

	vector<vec3> v;
	triangles.getPoints(v);
	vector<vec2> vt;
	triangles.getUVs(vt);
	vector<vec3> vn;
	triangles.getNormals(vn);
	vector<triangle> tris;
	triangles.getTriIndices(tris);
	uint max = tris.size() * 3;
	checkGLError(245);
	if (!max) {
		cout << "No Vertices To Load!";
		throw;
	}
	Varying buff[max];
	uint i = 0;
	for (triangle t : tris)
	{ // load in the location data each possibly empty except points
		for (uint j = 0; j < 3; ++j)
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
	glGenVertexArrays(1, &b.vao);
	glBindVertexArray(b.vao);
	glGenBuffers(1, &b.vbo);
	glBindBuffer(b.type, b.vbo);
	checkGLError(280);
	uint isize = sizeof(float) * 3 * n;
	glBufferData(b.type, b.bytesSize + (isize * 2), (GLvoid *) 0,
			GL_STATIC_DRAW);
	glBufferSubData(b.type, 0, b.bytesSize, &buff[0]);
	checkGLError(284);
	glVertexAttribPointer(VertLoc, 3, b.buffType, GL_FALSE, sizeof(Varying),
			(GLvoid *) 0);
	glEnableVertexAttribArray(VertLoc);
	glVertexAttribPointer(NormalLoc, 3, b.buffType, GL_FALSE, sizeof(Varying),
			(GLvoid *) (sizeof(float) * 3));
	glEnableVertexAttribArray(NormalLoc);
	glVertexAttribPointer(TexCoordsLoc, 2, b.buffType, GL_FALSE,
			sizeof(Varying), (GLvoid *) (sizeof(float) * 6));
	glEnableVertexAttribArray(TexCoordsLoc);
	// instancing attributes
	if (n > 0) // bypass if there are none
	{
		uint fs = 2 * 3 * n, k = 3 * n;
		float poscol[fs];
		for (uint i = 0; i < fs; ++i)
			poscol[i] = (i < k) ? pos[i] : col[i - k];
		glBufferSubData(b.type, b.bytesSize, isize * 2, &poscol[0]);
		glVertexAttribPointer(PositionLoc, 3, b.buffType, GL_FALSE, 0,
				(GLvoid *) b.bytesSize);
		glEnableVertexAttribArray(PositionLoc);
		glVertexAttribDivisor(PositionLoc, 1);
		glVertexAttribPointer(ColourLoc, 3, b.buffType, GL_FALSE, 0,
				(GLvoid *) (b.bytesSize + isize));
		glEnableVertexAttribArray(ColourLoc);
		glVertexAttribDivisor(ColourLoc, 1);
	}
	checkGLError(227);
	m_buffOb[b.vao] = b;
	return b.vao;
}
/******************************************************************************
 * Draw the contents of a buffer(s) on the video card
 * @param id opengl reference to the vertex array object
 * @param insts how many instances we will draw
 * TODO: create buffers for instances
 */
void Geometry::draw( uint id, GLsizei insts )
{
  checkGLError( 368 );
  EBuffer e;
  Buffer b;
  if ( m_elemBuffOb.find( id ) != m_elemBuffOb.end() )
  {
    e = m_elemBuffOb[id];
    checkGLError( 378 );
    glBindVertexArray( e.vao );
    if ( insts == 1 )
      glDrawElements( GL_TRIANGLES, e.eNumElements, e.eBuffType,
          (GLvoid *) 0 );
    else
      glDrawElementsInstanced( GL_TRIANGLES, e.eNumElements, e.eBuffType,
          (GLvoid *) 0, insts );
  }
  else if ( m_buffOb.find( id ) != m_buffOb.end() )
  {
    b = m_buffOb[id];
    glBindVertexArray( b.vao );
    if ( insts == 1 )
      glDrawArrays( GL_TRIANGLES, 0, b.numElements );
    else
      glDrawArraysInstanced( GL_TRIANGLES, 0, b.numElements, insts );
  }
  else
  {
    std::cout << "No Such Vertex Array Object (name:" << id << ")\n";
    throw;
  }
}

void Geometry::draw(uint id[], GLsizei insts[])
{
	// TODO:
}

void Geometry::drawAll()
{
	// TODO:
}

} /**< namespace vogl */

