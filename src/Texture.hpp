/*
 * Texture.h
 *
 *  Created on: 24/08/2015
 *      Author: stewarvivi
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <map>
#include <png.h>
#include <cstdio>
#include <SOIL/SOIL.h>

namespace vogl
{
  struct EMap
  {
	  GLuint fboID, rbID, textureID, depthCMID, colorCMID, res;
  };

  class Texture
  {
    public:
	  static Texture *getInstance();
      GLuint addPNGTexture( const std::string&, int&, int& );
      GLuint getPNGName( const std::string& );
      void makeEnviromentMap( const std::string&, uint, uint, uint );
      void setupEnvMap( const std::string&, uint, uint, uint );
      void createOmniView( glm::vec3, glm::mat4[6] );
      void renderEnvironmentMap( glm::vec3, std::string& );
      virtual ~Texture();
    private:
      Texture();
      static Texture *instance;
      std::map< std::string, GLuint > names;
      std::map< std::string, EMap > envir;
  };

} /* namespace vogl */

#endif /* TEXTURE_H_ */
