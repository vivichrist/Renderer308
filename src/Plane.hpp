/*
 * Plane.h
 *
 *  Created on: 4/02/2015
 *      Author: vivichrist
 */

#ifndef PLANE_HPP_
#define PLANE_HPP_

#include <glm/glm.hpp>

using namespace std;


namespace vogl
{

class Plane
{
public:
	// point in the plane, normal orthogonal to the plane
	Plane();
	Plane( const glm::vec3& point, const glm::vec3& normal );
	static Plane fromPoints( const glm::vec3& p0
	                       , const glm::vec3& p1
	                       , const glm::vec3& p2 );
	float distance( const glm::vec3& point );
	float distance( const Plane& point );
	glm::vec3 normal();
	glm::vec4 plane();
	virtual ~Plane();
private:
	glm::vec4 parameters;
	glm::vec3 point;
};

} // end vogl namespace

#endif /* PLANE_HPP_ */
