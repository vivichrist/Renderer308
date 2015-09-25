/*
 * Lights.cpp
 *
 *  Created on: 23/09/2015
 *      Author: stewarvivi
 */

#include "Lights.hpp"

using namespace glm;

namespace vogl
{

Lights::Lights()
{
	// TODO Auto-generated constructor stub
}

void Lights::addDirectionalLight( vec3 direction, vec3 color )
{
	mat4 light( vec4( direction, 0.0f ), vec4( color, 1.0f ), vec4(), vec4() );
	lights.push_back( light );
}

void Lights::addPointLight( vec3 position, vec3 color,
		float attConstant, float attLinear, float attQuadratic, float ambient )
{
	mat4 light( vec4( position, 1.0f )
				, vec4( color, 1.0f )
				, vec4( attConstant, attLinear, attQuadratic, ambient )
				, vec4( 0.0f, -1.0f, 0.0f, 180.0f ) );
	lights.push_back( light );
}

void Lights::addSpotLight( glm::vec3 position, glm::vec3 color,
		float attConstant, float attLinear, float attQuadratic, float ambient,
		glm::vec3 coneDir, float coneAngle )
{
	mat4 light( vec4( position, 1.0f )
				, vec4( color, 1.0f )
				, vec4( attConstant, attLinear, attQuadratic, ambient )
				, vec4( coneDir, coneAngle ) );
	lights.push_back( light );
}

void Lights::getLightMatrices( mat4 out[], uint& size )
{
	size = lights.size();
	out = lights.data();
}

Lights::~Lights()
{
	// TODO Auto-generated destructor stub
}

} /* namespace vogl */
