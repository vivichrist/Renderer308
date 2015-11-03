/*
 * Lights.hpp
 *
 *  Created on: 23/09/2015
 *      Author: stewarvivi
 */

#pragma once

#define GLM_FORCE_RADIANS

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

struct Light {
  glm::vec4 pos; // is also the direction of directional light
  glm::vec4 spec; // colour of specular highlights
  glm::vec4 att; // attenuation coefficients, ambient coefficient is w;
  glm::vec4 coneDir; // cone direction, cone angle is w;
};

namespace R308
{

  class Lights
  {
    public:
      static Lights *getInstance();
      uint addDirectionalLight( glm::vec3 direction, glm::vec3 color );
      uint addPointLight( glm::vec3 position, glm::vec3 color
    		  , float attConstant, float attLinear, float attQuadratic
    		  , float ambient );
      uint addSpotLight( glm::vec3 position, glm::vec3 color
    		  , float attConstant, float attLinear, float attQuadratic
    		  , float ambient, glm::vec3 coneDir, float coneAngle );
      glm::vec4 getPosition( const uint index );
      void moveLight( const glm::vec3& translate, const uint index );
      void directLight( const glm::mat3& M, const uint index );
      void beamLight( const float angle, const uint index );
      void getLights( Light[], GLint& );
      void getLights( float[160] , GLint& );
      virtual ~Lights();
    private:
      std::vector<Light> lights;
      static Lights *instance;
      Lights();
  };

} /* namespace vogl */
