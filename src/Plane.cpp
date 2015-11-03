/*
 * Plane.cpp
 *
 *  Created on: 4/02/2015
 *      Author: vivichrist
 */

#include "Plane.hpp"

using namespace std;
using namespace glm;

namespace R308
{

/**
 * Constructs a default plane centred at the origin and facing in the Y
 * direction.
 */
Plane::Plane()
{
	parameters = vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	point = vec3();
}
/**
 * Constructs a plane with the initial @param point and facing the
 * @param normal direction.
 */
Plane::Plane( const vec3& point, const vec3& normal )
{
	vec3 n = normalize( normal );
	// Ax + By + Cz + D = 0
	parameters = vec4( n, -glm::dot( n, point ) );
}
/**
 * @return The distance between this plane and the @param point
 */
float Plane::distance( const vec3& point )
{
	return ( dot( vec3( parameters ), point )
		   + parameters.w
	);
}
/**
 * @return The distance between this plane and another @param plane
 * from their initial points. Only useful if the planes are parallel.
 */
float Plane::distance( const Plane& plane )
{
	return ( dot( vec3( parameters ), plane.point )
		   + parameters.w
	);
}
/**
 * Constructs and @return a new Plane from three points on that plane. We get
 * the normal by using the cross product of the two line segments from @param
 * p0 to @param p1 and p2 (order is important here or the normal will be
 * wrong).
 */
Plane Plane::fromPoints( const vec3& p0, const vec3& p1, const vec3& p2 )
{
	return (
		Plane( p0, glm::cross( p1 - p0, p2 - p0 ) )
	);
}

glm::vec3 Plane::normal()
{
	return( glm::vec3( parameters ) );
}

glm::vec4 Plane::plane()
{
	return( parameters );
}

Plane::~Plane(){}

} // end vogl namespace
