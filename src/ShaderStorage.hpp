#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

namespace R308
{

class ShaderStorage
{
public:
	ShaderStorage( GLuint binding, uint size );
	~ShaderStorage();
	template <typename T>
	void setStorageData4( const uint& offset, const T* data, const size_t& size ) const;
	template <typename T>
	void setStorageData2( const uint& offset, const T* data, const size_t& size ) const;
	template <typename T>
	void setStorageData( const uint& offset, const T* data ) const;
	template <typename T>
	void setStorageDataArray(const uint &offset, const T *data, const size_t &size) const;
	void bindShaderStorage(const GLuint &shaderProgram, const std::string &block);

      private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}