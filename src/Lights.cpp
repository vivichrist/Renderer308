/*
 * Lights.cpp
 *
 *  Created on: 23/09/2015
 *      Author: stewarvivi
 */

#include "Lights.hpp"

using namespace glm;

namespace R308
{

Lights::Lights()
{
}

Lights *Lights::instance = nullptr;

Lights *Lights::getInstance()
{
	if ( instance == nullptr )
		instance = new Lights();
	return instance;
}

uint Lights::addDirectionalLight( vec3 direction, vec3 color )
{
	Light light;
	light.pos = vec4( direction, 0.0f );
	light.spec = vec4( color, 1.0f );
	light.att = vec4();
	light.coneDir = vec4();
	lights.push_back( light );
	return lights.size() - 1;
}

uint Lights::addPointLight( vec3 position, vec3 color, float attConstant,
		float attLinear, float attQuadratic, float ambient )
{
	Light light;
	light.pos = vec4( position, 1.0f );
	light.spec = vec4( color, 1.0f );
	light.att = vec4( attConstant, attLinear, attQuadratic, ambient );
	light.coneDir = vec4();
	lights.push_back( light );
	return lights.size() - 1;
}

uint Lights::addSpotLight( vec3 position, vec3 color,
		float attConstant, float attLinear, float attQuadratic, float ambient,
		glm::vec3 coneDir, float coneAngle )
{
	Light light;
	light.pos = vec4( position, 1.0f );
	light.spec = vec4( color, 1.0f );
	light.att = vec4( attConstant, attLinear, attQuadratic, ambient );
	light.coneDir = vec4( coneDir, coneAngle );
	lights.push_back( light );
	return lights.size() - 1;
}

vec4 Lights::getPosition( const uint index )
{
  return lights[index].pos;
}

void Lights::moveLight( const vec3& translate, const uint index )
{
  Light &light = lights[index];
  light.pos += vec4( translate, 0 );
}

void Lights::directLight( const mat3& rotM, const uint index )
{
  Light &light = lights[index];
  light.coneDir = vec4( rotM * vec3( 0.0f, -1.0f, 0.0f ), light.coneDir.w );
}

void Lights::beamLight( const float angle, const uint index )
{
  Light &light = lights[index];
  light.coneDir.w += angle;
}

void R308::Lights::getLights( float out[160], GLint& num )
{
	num = 0;
	for ( Light l : lights )
	{
		for ( int j = 0; j < 4; ++j )
			out[num + j] = l.pos[j];
		num += 4;
		for ( int j = 0; j < 4; ++j )
			out[num + j] = l.spec[j];
		num += 4;
		for ( int j = 0; j < 4; ++j )
			out[num + j] = l.att[j];
		num += 4;
		for ( int j = 0; j < 4; ++j )
			out[num + j] = l.coneDir[j];
		num += 4;
	}
	num /= 16;
}

void Lights::getLights( Light out[], GLint& size )
{
	size = lights.size();
	out = lights.data();
}

Lights::~Lights()
{
	lights.clear();
}

} /* namespace vogl */

