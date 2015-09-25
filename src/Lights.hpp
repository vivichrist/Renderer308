/*
 * Lights.hpp
 *
 *  Created on: 23/09/2015
 *      Author: stewarvivi
 */

#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Light {
  glm::vec4 pos; // is also the direction of directional light
  glm::vec4 spec; // colour of specular highlights
  glm::vec4 att; // attenuation coefficients, ambient coefficient is w;
  glm::vec4 coneDir; // cone direction, cone angle is w;
};

namespace vogl
{

  class Lights
  {
    private:
      std::vector<glm::mat4> lights;
    public:
      Lights();
      void addDirectionalLight( glm::vec3 direction, glm::vec3 color );
      void addPointLight( glm::vec3 position, glm::vec3 color
    		  , float attConstant, float attLinear, float attQuadratic
			  , float ambient );
      void addSpotLight( glm::vec3 position, glm::vec3 color
    		  , float attConstant, float attLinear, float attQuadratic
			  , float ambient, glm::vec3 coneDir, float coneAngle );
      void getLightMatrices( glm::mat4[], uint& );
      virtual ~Lights();
  };

} /* namespace vogl */
