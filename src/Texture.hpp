/*
 * Texture.h
 *
 *  Created on: 24/08/2015
 *      Author: stewarvivi
 */

#pragma once

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <stb_image.h>
#include "Shader.hpp"

namespace vogl
{
  struct image
  {

      int w, h, n;
      std::vector<unsigned char> data;

      image( int w_, int h_, int n_ )
          : w( w_ ), h( h_ ), n( n_ ), data( w * h * n, 0 )
      {
      }

      image( const std::string &filepath )
      {
        unsigned char *stbi_data = stbi_load( filepath.c_str(), &w, &h, &n, 0 );
        if ( !stbi_data )
        {
          throw std::runtime_error( "Failed load image from " + filepath );
        }
        data.assign( stbi_data, stbi_data + (w * h * n) );
        stbi_image_free( stbi_data );
      }

      image( const image & ) = default;
      image & operator=( const image & ) = default;
      image( image && ) = default;
      image & operator=( image && ) = default;

      // Use to get the appropriate GL format for data
      GLenum glFormat() const
      {
        switch ( n )
        {
          case 1:
            return GL_R;
          case 2:
            return GL_RG;
          case 3:
            return GL_RGB;
          case 4:
            return GL_RGBA;
          default:
            return GL_RGB; // TODO list
        }
      }

      // Use to get a GL friendly pointer to the data
      unsigned char * dataPointer()
      {
        return &data[ 0 ];
      }
      const unsigned char * dataPointer() const
      {
        return &data[ 0 ];
      }

      image subImage( int xoffset, int yoffset, int width, int height )
      {
        image r( width, height, n );

        for ( int y = 0; y < height; y++ )
        {
          if ( (y + yoffset) >= h )
            continue;
          for ( int x = 0; x < width; x++ )
          {
            if ( (x + xoffset) >= w )
              continue;
            for ( int i = 0; i < n; i++ )
            {
              r.data[ (y * width * n) + (x * n) + i ] = data[ ((y + yoffset) * w
                  * n) + ((x + xoffset) * n) + i ];
            }
          }
        }
        return r;
      }
  };

  struct EMap
  {
      GLuint fboID, rbID, textureID, depthCMID, colorCMID, res;
      Shader shader;
  };

  class Texture
  {
    public:
      static Texture *getInstance();
      GLuint addTexture( const std::string& );
      GLuint addCMTexture( const std::string& );
      GLuint addTexture( const glm::vec3& );
      GLuint getPNGName( const std::string& );
      void makeEnviromentMap( const std::string&, uint );
      GLuint setupEnvMap( const std::string&, uint );
      GLuint getEnvMap( const std::string& );
      void createOmniView( const glm::vec3&, glm::mat4[ 6 ], glm::mat3[ 6 ] );
      void useEnvironmentMap( Shader*&, glm::vec3, const std::string& );
      uint unUseEnvironmentMap( uint width, uint height, const std::string& );
      virtual ~Texture();
    private:
      Texture();
      static Texture *instance;
      std::map<std::string, GLuint> names;
      std::map<std::string, EMap> envir;
  };

} /* namespace vogl */
