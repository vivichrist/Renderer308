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
	UniformBlock( GLuint binding, uint size );
	~UniformBlock();
	void setUniformDataf( GLfloat const *data, uint const &col, uint const &row, uint const &offset );
	void setUniformDatai( GLint const *data, uint const &col, uint const &row, uint const &offset );
	void setUniformDataui(GLuint const *data, uint const &col, uint const &row, uint const &offset);
	void bindUniformBlock(const GLuint &shaderProgram, const std::string &block);

      private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}