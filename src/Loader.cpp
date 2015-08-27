/**************************************************************************//**
 * @class Loader
 * @file Loader.cpp
 * @brief ...
 *  Created on: 18/08/2015
 *      Author: vivichrist
 *****************************************************************************/

#include "Loader.hpp"

using namespace std;
using namespace glm;

namespace vogl
{

Loader::Loader()
{
}

void Loader::readOBJ( const string &filename )
{

	// Make sure our geometry information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();
	m_lines.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back( vec3( 0, 0, 0 ) );
	m_uvs.push_back( vec2( 0, 0 ) );
	m_normals.push_back( vec3( 0, 0, 1 ) );

	ifstream objFile( filename );

	if ( !objFile.is_open() )
	{
		cerr << "Error reading " << filename << endl;
		throw runtime_error( "Error :: could not open file." );
	}

	cout << "Reading file " << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while ( objFile.good() )
	{
		// Pull out line from file
		string line;
		std::getline( objFile, line );
		istringstream objLine( line );

		// Pull out mode from line
		string mode;
		objLine >> mode;
		if ( mode[0] != 'v' && mode[0] != 'f' )
			continue;

		// Reading like this means whitespace at the start of the line is fine
		// attempting to read from an empty string/line will set the failbit
		if ( !objLine.fail() )
		{

			if ( mode == "v" )
			{
				vec3 v;
				objLine >> v.x >> v.y >> v.z;
				m_points.push_back( v );
			}
			else if ( mode == "vn" )
			{
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				m_normals.push_back( vn );
			}
			else if ( mode == "vt" )
			{
				vec2 vt;
				objLine >> vt.x >> vt.y;
				m_uvs.push_back( vt );
			}
			else if ( mode == "f" )
			{
				vector< vertex > verts;
				while ( objLine.good() )
				{
					vertex v;

					objLine >> v.p;		// Scan in position index
					if ( objLine.peek() == '/' )
					{
						objLine.ignore( 1 );	// Ignore the '/' character
						if ( objLine.peek() != '/' )
						{
							objLine >> v.t;	// Scan in uv (texture coord) index
						}
						// Scan in normal index
						objLine.ignore( 1 );	// Ignore the '/' character
						objLine >> v.n;
					}
					verts.push_back( v );
				}
				// IFF we have 4 indices, construct a triangle from the quad
				if ( verts.size() == 3 )
				{
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back( tri );
					edge l; // triangle outline
					l.p1 = verts[0].p;
					l.p2 = verts[1].p;
					m_lines.push_back( l );
					l.p1 = verts[1].p;
					l.p2 = verts[2].p;
					m_lines.push_back( l );
					l.p1 = verts[2].p;
					l.p2 = verts[0].p;
					m_lines.push_back( l );
				}
				else if ( verts.size() == 4 )
				{
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back( tri );
					tri.v[0] = verts[0];
					tri.v[1] = verts[2];
					tri.v[2] = verts[3];
					m_triangles.push_back( tri );
					edge l; // quad outline
					l.p1 = verts[0].p;
					l.p2 = verts[1].p;
					m_lines.push_back( l );
					l.p1 = verts[1].p;
					l.p2 = verts[2].p;
					m_lines.push_back( l );
					l.p1 = verts[2].p;
					l.p2 = verts[3].p;
					m_lines.push_back( l );
					l.p1 = verts[3].p;
					l.p2 = verts[0].p;
					m_lines.push_back( l );
				}
			}
		}
	}

	cout << "Reading OBJ file is DONE." << endl;
	cout << m_points.size() - 1 << " points" << endl;
	cout << m_uvs.size() - 1 << " uv coords" << endl;
	cout << m_normals.size() - 1 << " normals" << endl;
	cout << m_triangles.size() << " faces" << endl;

	// If we didn't have any normals, create them
//	if ( m_normals.size() <= 1 )
//		createNormals();
}

void Loader::createNormals()
{
	ushort idx = 0, max = 0;
	map< ushort, set< ushort > > relation;
	vec3 sum;
	// store collected surface normals in this array
	vec3 faceNormals[m_triangles.size()] = { vec3( 0 ) };
	// find all surrounding surface normals
	for ( auto &tri : m_triangles )
	{
		ushort v0 = tri.v[0].p, v1 = tri.v[1].p, v2 = tri.v[2].p;
		vec3 norm = cross( m_points[v1] - m_points[v0],
				m_points[v2] - m_points[v0] );
		tri.v[0].n = v0; // point and normal indices match
		tri.v[1].n = v1;
		tri.v[2].n = v2;
		faceNormals[idx] = normalize( norm );
		relation[v0].insert( idx ); // note which face normals are adjacent.
		relation[v1].insert( idx );
		relation[v2].insert( idx );
		++idx;
	}
	max = (ushort) relation.size(); // 1 less than m_points
	for ( idx = 1; idx <= max; ++idx )
	{ // vertex normal creation
		sum = vec3();
		for ( ushort norm : relation[idx] ) // relation indexed from 1
			sum += faceNormals[norm]; // sum of face normals indexed from 0
		m_normals.push_back( normalize( sum ) );
	}
}

void Loader::getPoints( vector< vec3 > &points )
{
	points = vector< vec3 >( m_points );
}

void Loader::getNormals( vector< vec3 > &norms )
{
	norms = vector< vec3 >( m_normals );
}

void Loader::getUVs( vector< vec2 > &uvs )
{
	uvs = vector< vec2 >( m_uvs );
}

void Loader::getTriIndices( vector<triangle> &ind )
{
	ind = vector<triangle>( m_triangles );
}

void Loader::getLinesInd( std::vector< GLushort >& ind )
{
	for ( edge e : m_lines )
	{
		ind.push_back( e.p1 );
		ind.push_back( e.p2 );
	}
}

Loader::~Loader()
{
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();
	m_lines.clear();
}

} /**< namespace vogl */
