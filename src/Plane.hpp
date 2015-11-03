/*
 * Plane.h
 *
 *  Created on: 4/02/2015
 *      Author: vivichrist
 */

#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

using namespace std;


namespace R308
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
