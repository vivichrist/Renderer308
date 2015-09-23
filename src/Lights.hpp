/*
 * Lights.hpp
 *
 *  Created on: 23/09/2015
 *      Author: stewarvivi
 */

#pragma once
#include <glm/glm.hpp>

struct Light {
  glm::vec4 pos;
  glm::vec4 spec; // color of specular highlights
  glm::vec4 att; // attenuation coefficients, ambient coefficient is w;
  glm::vec4 coneDir; // cone direction, cone angle is w;
};

namespace vogl
{

  class Lights
  {
    private:
      std::vector<Light> lights;
    public:
      Lights();
      addPointLight( glm::vec3 position, glm::vec3 color, glm::vec3 attenuation );
      addSpotLight( glm::vec3 position, glm::vec3 attenuation,  );
      virtual ~Lights();
  };

} /* namespace vogl */
