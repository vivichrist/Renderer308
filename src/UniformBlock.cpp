#include "UniformBlock.hpp"

namespace R308
{
// Size is the number of slots of 4 GLfloats to be buffered
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

// both offset and size are number of slots of 4 GLfloats
void UniformBlock::setUniformData4( const uint& offset, const void* data, const size_t& size ) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	uint obytes = offset * 4u * sizeof(GLfloat);
	uint sbytes = size * 4u * sizeof(GLfloat);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data);
}
// both offset and size are number of slots of 2 GLfloats
void UniformBlock::setUniformData2( const uint& offset, const void* data, const size_t& size ) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	uint obytes = offset * 2u * sizeof(GLfloat);
	uint sbytes = size * 2u * sizeof(GLfloat);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data);
}
// both offset and size are number of slots of one GLfloat or scalar members
void UniformBlock::setUniformData( const uint& offset, const void* data, const size_t& size ) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	uint obytes = offset * sizeof(GLfloat);
	uint sbytes = size * sizeof(GLfloat);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data);
}

void UniformBlock::bindUniformBlock( const GLuint& shaderProgram, const std::string& block )
{
	// index of binding point. Other shaders with same uniform block should use this binding point
	// get index of uniform buffer from shader
	GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, block.c_str() );
	// bind uniform block to binding point
	glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
}

}