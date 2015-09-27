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

Lights::Lights() {}

Lights *Lights::instance = nullptr;

Lights *Lights::getInstance()
{
  if (instance == nullptr)
    instance = new Lights();
  return instance;
}

void Lights::addDirectionalLight( vec3 direction, vec3 color )
{
  Light light;
  light.pos = vec4( direction, 0.0f );
  light.spec = vec4( color, 1.0f );
  light.att = vec4();
  light.coneDir = vec4();
	lights.push_back( light );
}

void Lights::addPointLight( vec3 position, vec3 color,
		float attConstant, float attLinear, float attQuadratic, float ambient )
{
  Light light;
  light.pos = vec4( position, 1.0f );
  light.spec = vec4( color, 1.0f );
	light.att = vec4( attConstant, attLinear, attQuadratic, ambient );
	light.coneDir = vec4( 0.0f, -1.0f, 0.0f, 180.0f );
	lights.push_back( light );
}

void Lights::addSpotLight( glm::vec3 position, glm::vec3 color,
		float attConstant, float attLinear, float attQuadratic, float ambient,
		glm::vec3 coneDir, float coneAngle )
{
  Light light;
  light.pos = vec4( position, 1.0f );
  light.spec = vec4( color, 1.0f );
  light.att = vec4( attConstant, attLinear, attQuadratic, ambient );
  light.coneDir = vec4( coneDir, coneAngle );
	lights.push_back( light );
}

void vogl::Lights::getLights( float out[160], GLint& num )
{
  num = 0;
  for ( Light l: lights )
  {
    for ( int j = 0; j<4; ++j )
      out[num + j] = l.pos[j];
    num += 4;
    for ( int j = 0; j<4; ++j )
      out[num + j] = l.spec[j];
    num += 4;
    for ( int j = 0; j<4; ++j )
      out[num + j] = l.att[j];
    num += 4;
    for ( int j = 0; j<4; ++j )
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

