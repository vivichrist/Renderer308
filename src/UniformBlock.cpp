#include "UniformBlock.hpp"

namespace R308
{
// Size is the number of slots of vec4 to be buffered
UniformBlock::UniformBlock( GLuint binding, uint size )
{
	bindingPoint = binding;
	// create new buffer for Uniform Buffer Object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	// bind buffer to binding point
	GLsizei s = sizeof(GLfloat) * 4u * size;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
	glBufferData(GL_UNIFORM_BUFFER, (GLsizei) s, nullptr, GL_DYNAMIC_DRAW);
}

UniformBlock::~UniformBlock()
{
}

// both offset and size are number of slots of vec4 so size==4 -> mat4
// size==3 -> mat3 or mat3x4 or mat3x2 (assumes data is padded out)
void UniformBlock::setUniformDataf( GLfloat const *data, uint const &col, uint const &row, uint const &offset )
{
	uint obytes, sbytes;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(GLfloat);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(GLfloat);
			glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(GLfloat);
	obytes = offset * col * sizeof(GLfloat);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data );
}

void UniformBlock::setUniformDatai( GLint const *data, uint const &col, uint const &row, uint const &offset )
{
	uint obytes, sbytes;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(GLint);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(GLint);
			glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(GLint);
	obytes = offset * col * sizeof(GLint);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data );
}

void UniformBlock::setUniformDataui( GLuint const *data, uint const &col, uint const &row, uint const &offset )
{
	uint obytes, sbytes;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(GLuint);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(GLuint);
			glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(GLuint);
	obytes = offset * col * sizeof(GLuint);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data );
}

void UniformBlock::bindUniformBlock( const GLuint& shaderProgram, const std::string& block )
{
	// index of binding point. Other shaders with same uniform block should use this binding point
	// get index of uniform buffer from shader
	GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, block.c_str() );
	// bind uniform block to binding point
	glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
}

} // end R308 namespace