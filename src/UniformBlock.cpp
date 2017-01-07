#include "UniformBlock.hpp"

namespace R308
{
// Size is the number of slots of vec4 to be buffered
UniformBlock::UniformBlock( GLuint const& binding, size_t const& size )
{
	bindingPoint = binding;
	// create new buffer for Uniform Buffer Object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	// bind buffer to binding point
	GLsizei s = 16u * size;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
	glBufferData(GL_UNIFORM_BUFFER, (GLsizei) s, nullptr, GL_DYNAMIC_DRAW);
}

UniformBlock::~UniformBlock()
{
	glDeleteBuffers(1u, &buffer);
}

// both offset and size are number of slots of vec4 so size==4 -> mat4
// size==3 -> mat3 or mat3x4 or mat3x2 (assumes data is padded out)
template<typename T>
void UniformBlock::setUniformData( T const *data, uint const &col, uint const &row, uint const &offset )
{
	uint obytes, sbytes;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(T);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(T);
			glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(T);
	obytes = offset * col * sizeof(T);
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

template void UniformBlock::setUniformData<GLfloat>(GLfloat const *, uint const &, uint const &, uint const &);
template void UniformBlock::setUniformData<GLuint>(GLuint const *, uint const &, uint const &, uint const &);
template void UniformBlock::setUniformData<GLint>(GLint const *, uint const &, uint const &, uint const &);

} // end R308 namespace