#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

namespace R308
{

class UniformBlock
{
public:
	UniformBlock( GLuint binding, uint size );
	~UniformBlock();
	void setUniformData4( const uint& offset, const void* data, const size_t& size ) const;
	void setUniformData2( const uint& offset, const void* data, const size_t& size ) const;
	void setUniformData( const uint& offset, const void* data, const size_t& size ) const;
	void bindUniformBlock( const GLuint& shaderProgram, const std::string& block );
private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}