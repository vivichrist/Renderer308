#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <typeinfo>
#include <string>

#pragma once

namespace R308
{

class UniformBlock
{
public:
	UniformBlock( GLuint binding, uint size );
	~UniformBlock();
	template <typename T>
	void setUniformData( const uint &col, const uint &row, const uint &offset, T *data );
	void bindUniformBlock(const GLuint &shaderProgram, const std::string &block);

private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}