/**************************************************************************//**
 * @class Loader
 * @file Loader.h
 * @brief ...
 *  Created on: 18/08/2015
 *      Author: vivichrist
 *****************************************************************************/
#pragma once
#define GLM_FORCE_RADIANS

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <set>

namespace vogl
{

struct vertex {
	GLushort p = 0; // index for point in m_points
	GLushort t = 0; // index for uv in m_uvs
	GLushort n = 0; // index for normal in m_normals
};

struct triangle {
	vertex v[3]; //requires 3 verticies
};

struct edge {
	GLushort p1;
	GLushort p2;//requires 2 points
};

class Loader
{
private:
	// Feilds for storing raw obj information
	std::string m_filename;
	std::vector<glm::vec3> m_points;	// Point list
	std::vector<edge> m_lines;
	std::vector<glm::vec2> m_uvs;		// Texture Coordinate list
	std::vector<glm::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;		// Triangle/Face list
public:
	Loader();
	void createNormals();
	void readOBJ( const std::string& );
	void getPoints( std::vector<glm::vec3>& );
	void getNormals( std::vector<glm::vec3>& );
	void getUVs( std::vector<glm::vec2>& );
	void getTriIndices( std::vector<triangle>& );
	void getLinesInd( std::vector<GLushort>& );
	virtual ~Loader();
	/**< @class Loader */
};

} /**< namespace vogl */
