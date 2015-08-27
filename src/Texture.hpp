/*
 * Texture.h
 *
 *  Created on: 24/08/2015
 *      Author: stewarvivi
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <map>

namespace vogl
{

  class Texture
  {
    public:
      Texture();
      GLuint addPNGTexture( const std::string&, int&, int& );
      GLuint getPNGName( const std::string& );
      virtual ~Texture();
    private:
      std::map< std::string, GLuint > names;
  };

} /* namespace vogl */

#endif /* TEXTURE_H_ */
