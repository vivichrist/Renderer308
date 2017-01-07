#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <typeinfo>
#include <string>


namespace R308
{

class UniformBlock
{
public:
	UniformBlock( GLuint const& binding, size_t const& size );
	~UniformBlock();
	template<typename T>
	void setUniformData( T const *data, uint const &col, uint const &row, uint const &offset );
	void bindUniformBlock(const GLuint &shaderProgram, const std::string &block);

private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}